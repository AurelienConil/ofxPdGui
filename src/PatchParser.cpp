/**
 * @file PatchParser.cpp
 * @brief Implémentation du système de parsing des fichiers Pure Data
 * 
 * Ce fichier contient l'implémentation complète du parser qui convertit
 * les fichiers .pd en objets C++ utilisables par le système de rendu.
 * 
 * ARCHITECTURE DU PARSING :
 * 1. parseFile() : Lecture ligne par ligne du fichier .pd
 * 2. parseLine() : Analyse syntaxique et dispatch selon le type
 * 3. parseXXX() : Parsers spécialisés pour chaque type d'objet
 * 
 * SYNTAXE PURE DATA SUPPORTÉE :
 * - #X obj x y type params... : Objets GUI standards
 * - #X floatatom x y params... : Boîtes de nombre  
 * - #N canvas x y width height : Sous-patches (à venir)
 * 
 * GESTION DES ERREURS :
 * - Try/catch pour chaque ligne de parsing
 * - Logs d'erreur détaillés pour le debugging
 * - Poursuite du parsing même en cas d'erreur sur une ligne
 * 
 * OPTIMISATIONS :
 * - Utilisation d'ofBuffer pour la lecture efficace
 * - Smart pointers pour la gestion mémoire
 * - Filtrage des objets sans send/receive (objets purement visuels)
 */

//
//  PatchParser.cpp
//  pd-gui
//
//  Created by Aurélien Conil on 15/07/2025.
//

#include "PatchParser.h"
#include "Subpatch.h"

using namespace std;

/**
 * @brief Parse un fichier .pd complet et retourne tous les objets GUI
 * 
 * PROCESSUS :
 * 1. Chargement du fichier via ofBuffer (gestion UTF-8 et encodages)
 * 2. Parsing ligne par ligne avec gestion d'erreurs
 * 3. Filtrage automatique des objets non-GUI
 * 4. Création des objets C++ correspondants
 * 
 * @param filename Nom du fichier .pd (relatif à bin/data/)
 * @return Collection d'objets GUI prêts pour le rendu
 */
vector<unique_ptr<PdGuiObject>> PdPatchParser::parseFile(const string& filename) {
    vector<unique_ptr<PdGuiObject>> objects;
    
    // === CHARGEMENT DU FICHIER ===
    // ofBuffer gère automatiquement les différents encodages et sauts de ligne
    ofBuffer buffer = ofBufferFromFile(filename);
    
    if(buffer.size() == 0) {
        ofLogError("PdPatchParser") << "Cannot open file or file is empty: " << filename;
        return objects;
    }
    
    // === PARSING LIGNE PAR LIGNE ===
    // Chaque ligne du fichier .pd représente un élément du patch
    for(auto line : buffer.getLines()) {
        auto obj = parseLine(line);
        if(obj) {
            objects.push_back(move(obj));
        }
    }
    
    ofLogNotice("PdPatchParser") << "Successfully parsed " << objects.size() << " GUI objects from " << filename;
    return objects;
}

/**
 * @brief Analyse une ligne du fichier .pd et crée l'objet GUI correspondant
 * 
 * FORMATS SUPPORTÉS :
 * - #X obj x y type params... : Objets GUI (hsl, vsl, tgl, bng, cnv)
 * - #X floatatom x y params... : Boîtes de nombre
 * - Autres lignes : Ignorées (connexions, commentaires, etc.)
 * 
 * DISPATCH :
 * Selon le type détecté, appelle le parser spécialisé approprié.
 * 
 * @param line Ligne à analyser du fichier .pd
 * @return Objet GUI créé ou nullptr si la ligne n'est pas un objet GUI
 */
unique_ptr<PdGuiObject> PdPatchParser::parseLine(const string& line) {
    vector<string> tokens = splitString(line, ' ');
    if(tokens.size() < 3) return nullptr;
    
    try {
        // === OBJETS GUI STANDARDS : #X obj x y type params... ===
        if(line.find("#X obj") == 0) {
            // Format général : #X obj x y type params...
            if(tokens.size() < 5) return nullptr;
            
            float x = ofToFloat(tokens[2]);
            float y = ofToFloat(tokens[3]);
            string type = tokens[4];
            
            // Dispatch selon le type d'objet
            if(type == "hsl") {
                return parseHorizontalSlider(tokens, ofVec2f(x, y));
            } else if(type == "vsl") {
                return parseVerticalSlider(tokens, ofVec2f(x, y));
            } else if(type == "tgl") {
                return parseToggle(tokens, ofVec2f(x, y));
            } else if(type == "bng") {
                return parseBang(tokens, ofVec2f(x, y));
            } else if(type == "cnv") {
                return parseCanvas(tokens, ofVec2f(x, y));
            } else if(type == "pd") {
                // Subpatch reference : #X restore x y pd subpatch_name
                return parseSubpatch(tokens, ofVec2f(x, y));
            }
        }
        // === BOÎTES DE NOMBRE : #X floatatom x y params... ===
        else if(line.find("#X floatatom") == 0) {
            // Format : #X floatatom x y width min max label_pos label font_size send receive label
            if(tokens.size() < 4) return nullptr;
            
            float x = ofToFloat(tokens[2]);
            float y = ofToFloat(tokens[3]);
            
            return parseNumberBox(tokens, ofVec2f(x, y));
        }
        // === SUBPATCHES : #X restore x y pd name ===
        else if(line.find("#X restore") == 0) {
            // Format : #X restore x y pd subpatch_name
            if(tokens.size() < 5) return nullptr;
            
            float x = ofToFloat(tokens[2]);
            float y = ofToFloat(tokens[3]);
            
            return parseSubpatch(tokens, ofVec2f(x, y));
        }
    } catch(exception& e) {
        ofLogError("PdPatchParser") << "Error parsing line: " << line << " - " << e.what();
    }
    
    return nullptr;
}



/**
 * @brief Parse un slider horizontal Pure Data (hsl)
 * 
 * FORMAT PURE DATA :
 * #X obj x y hsl width height min max lin_log iem_init_i send receive label x_off y_off font font_size bg_color fg_color label_color val;
 * 
 * PARAMÈTRES IMPORTANTS :
 * - width/height : Dimensions du slider  
 * - min/max : Plage de valeurs
 * - send/receive : Symboles de communication
 * - val : Valeur initiale (souvent en fin de ligne)
 * 
 * @param tokens Tokens de la ligne parsée
 * @param pos Position (x,y) de l'objet
 * @return Slider horizontal ou nullptr si erreur
 */
unique_ptr<PdGuiObject> PdPatchParser::parseHorizontalSlider(const vector<string>& tokens, ofVec2f pos) {
    // Format complet attendu avec au moins 12 paramètres de base
    if(tokens.size() < 12) return nullptr;
    
    string sendSym = tokens[10];
    string receiveSym = tokens[11];
    
    // === FILTRAGE DES OBJETS SANS COMMUNICATION ===
    // Ignorer les objets purement décoratifs (sans send/receive)
    if(sendSym == "empty" && receiveSym == "empty") return nullptr;
    
    // === EXTRACTION DES PARAMÈTRES ===
    ofVec2f size = ofVec2f(ofToFloat(tokens[5]), ofToFloat(tokens[6]));
    float minVal = ofToFloat(tokens[7]);
    float maxVal = ofToFloat(tokens[8]);
    
    // === VALEUR INITIALE ===
    // Pure Data place souvent la valeur initiale en fin de ligne
    float initialValue = minVal; // Valeur par défaut au minimum
    if(tokens.size() > 20) {
        // Récupérer la valeur du dernier token (convention Pure Data)
        initialValue = ofToFloat(tokens[tokens.size() - 1]);
        // S'assurer que la valeur est dans la plage autorisée
        initialValue = ofClamp(initialValue, minVal, maxVal);
    }
    
    return make_unique<PdSlider>(
        GuiType::HORIZONTAL_SLIDER,
        pos,
        size,
        sendSym,
        receiveSym,
        minVal,
        maxVal,
        initialValue
    );
}

unique_ptr<PdGuiObject> PdPatchParser::parseVerticalSlider(const vector<string>& tokens, ofVec2f pos) {
    // Format: #X obj x y vsl width height min max lin_log iem_init_i send receive label x_off y_off font font_size bg_color fg_color label_color val;
    if(tokens.size() < 12) return nullptr;
    
    string sendSym = tokens[10];
    string receiveSym = tokens[11];
    
    // Ignorer les objets sans send/receive
    if(sendSym == "empty" && receiveSym == "empty") return nullptr;
    
    ofVec2f size = ofVec2f(ofToFloat(tokens[5]), ofToFloat(tokens[6]));
    float minVal = ofToFloat(tokens[7]);
    float maxVal = ofToFloat(tokens[8]);
    
    // Valeur initiale (si disponible dans les tokens)
    float initialValue = minVal; // Valeur par défaut
    if(tokens.size() > 20) {
        // La valeur initiale est souvent dans le dernier token pour les sliders PD
        initialValue = ofToFloat(tokens[tokens.size() - 1]);
        // S'assurer que la valeur est dans la plage
        initialValue = ofClamp(initialValue, minVal, maxVal);
    }
    
    return make_unique<PdSlider>(
        GuiType::VERTICAL_SLIDER,
        pos,
        size,
        sendSym,
        receiveSym,
        minVal,
        maxVal,
        initialValue
    );
}

unique_ptr<PdGuiObject> PdPatchParser::parseToggle(const vector<string>& tokens, ofVec2f pos) {
    if(tokens.size() < 10) return nullptr;
    
    string sendSym = tokens[8];
    string receiveSym = tokens[9];
    
    if(sendSym == "empty" && receiveSym == "empty") return nullptr;
    
    ofVec2f size = ofVec2f(ofToFloat(tokens[5]), ofToFloat(tokens[5])); // Carré
    
    return make_unique<PdToggle>(pos, size, sendSym, receiveSym);
}

unique_ptr<PdGuiObject> PdPatchParser::parseBang(const vector<string>& tokens, ofVec2f pos) {
    if(tokens.size() < 9) return nullptr;
    
    string sendSym = tokens[7];
    string receiveSym = tokens[8];
    
    if(sendSym == "empty" && receiveSym == "empty") return nullptr;
    
    ofVec2f size = ofVec2f(ofToFloat(tokens[5]), ofToFloat(tokens[5]));
    
    return make_unique<PdBang>(pos, size, sendSym, receiveSym);
}

unique_ptr<PdGuiObject> PdPatchParser::parseNumberBox(const vector<string>& tokens, ofVec2f pos) {
    // Format: #X floatatom x y width min max label_pos label font_size send receive label
    // Exemple: #X floatatom 91 112 5 0 0 0 - - - 0;
    if(tokens.size() < 7) return nullptr;
    
    float width = ofToFloat(tokens[4]) * 8; // Largeur en caractères * largeur approximative d'un caractère
    float height = 20; // Hauteur standard
    ofVec2f size(width, height);
    
    float minVal = ofToFloat(tokens[5]);
    float maxVal = ofToFloat(tokens[6]);
    
    // Si min == max == 0, utiliser une plage par défaut
    if(minVal == 0 && maxVal == 0) {
        minVal = -1000000.0f;
        maxVal = 1000000.0f;
    }
    
    // Symboles send/receive (peuvent être "-" pour indiquer vide)
    string sendSym = "";
    string receiveSym = "";
    
    if(tokens.size() > 9 && tokens[9] != "-") {
        sendSym = tokens[9];
    }
    
    if(tokens.size() > 10 && tokens[10] != "-") {
        receiveSym = tokens[10];
    }
    
    // Si pas de symboles, on peut quand même créer l'objet pour l'affichage
    // mais on utilise des symboles génériques
    if(sendSym.empty() && receiveSym.empty()) {
        sendSym = "floatatom-" + ofToString(pos.x) + "-" + ofToString(pos.y);
        receiveSym = sendSym;
    }
    
    // Valeur initiale (souvent dans le dernier token)
    float initialValue = 0.0f;
    if(tokens.size() > 11) {
        string lastToken = tokens[tokens.size() - 1];
        // Enlever le point-virgule final si présent
        if(lastToken.back() == ';') {
            lastToken.pop_back();
        }
        initialValue = ofToFloat(lastToken);
        initialValue = ofClamp(initialValue, minVal, maxVal);
    }
    
    // Déterminer la précision basée sur la valeur initiale ou utiliser 2 par défaut
    int precision = 2;
    if(initialValue == floor(initialValue)) {
        precision = 0; // Nombre entier
    }
    
    return make_unique<PdNumberBox>(
        pos,
        size,
        sendSym,
        receiveSym,
        minVal,
        maxVal,
        initialValue,
        precision
    );
}

unique_ptr<PdGuiObject> PdPatchParser::parseCanvas(const vector<string>& tokens, ofVec2f pos) {
    // Format: #X obj x y cnv size width height send receive label x_off y_off font font_size bg_color fg_color label_color
    // Exemple: #X obj 167 111 cnv 19 126 76 empty empty empty 20 12 0 12 #ff0400 #ffffff 0;
    if(tokens.size() < 7) return nullptr;
    
    // Taille du canvas
    float width = ofToFloat(tokens[6]);
    float height = ofToFloat(tokens[7]);
    ofVec2f size(width, height);
    
    // Label (peut être "empty")
    string label = "";
    if(tokens.size() > 10 && tokens[10] != "empty") {
        label = tokens[10];
    }
    
    // Couleurs par défaut
    ofColor backgroundColor(224, 224, 224); // Gris clair par défaut
    ofColor textColor(0, 0, 0);             // Noir par défaut pour le texte
    
    // CORRECTION: Parser la couleur de fond (bg_color) - tokens[15]
    if(tokens.size() > 15) {
        string bgColorStr = tokens[15];
        if(bgColorStr.length() > 1 && bgColorStr[0] == '#') {
            backgroundColor = parseHexColor(bgColorStr);
        }
    }
    
    // CORRECTION: Parser la couleur de texte (fg_color) - tokens[16]
    if(tokens.size() > 16) {
        string textColorStr = tokens[16];
        if(textColorStr.length() > 1 && textColorStr[0] == '#') {
            textColor = parseHexColor(textColorStr);
        }
    }
    
    return make_unique<PdCanvas>(pos, size, label, backgroundColor, textColor);
}

// Méthode utilitaire pour parser les couleurs hexadécimales
ofColor PdPatchParser::parseHexColor(const string& hexStr) {
    if(hexStr.length() < 7 || hexStr[0] != '#') {
        ofLogWarning("PdCanvas") << "Invalid hex color format: " << hexStr;
        return ofColor(128, 128, 128); // Gris par défaut
    }
    
    try {
        // Enlever le #
        string hex = hexStr.substr(1);
        
        // S'assurer qu'on a exactement 6 caractères
        if(hex.length() != 6) {
            ofLogWarning("PdCanvas") << "Hex color should be 6 characters: " << hexStr;
            return ofColor(128, 128, 128);
        }
        
        // Convertir en valeurs RGB
        int r = stoi(hex.substr(0, 2), nullptr, 16);
        int g = stoi(hex.substr(2, 2), nullptr, 16);
        int b = stoi(hex.substr(4, 2), nullptr, 16);
        
        ofLogNotice("PdCanvas") << "Parsed color " << hexStr
                               << " -> R:" << r << " G:" << g << " B:" << b;
        
        return ofColor(r, g, b);
    } catch(exception& e) {
        ofLogError("PdPatchParser") << "Error parsing hex color: " << hexStr << " - " << e.what();
        return ofColor(128, 128, 128);
    }
}

/**
 * @brief Parse un subpatch Pure Data
 * 
 * FORMAT PURE DATA :
 * #X restore x y pd subpatch_name
 * 
 * FONCTIONNEMENT :
 * - Détecte la référence au subpatch dans le patch parent
 * - Construit le chemin vers le fichier .pd du subpatch
 * - Crée un objet PdSubpatch qui chargera et intégrera les objets enfants
 * 
 * @param tokens Tokens de la ligne parsée
 * @param pos Position (x,y) du subpatch dans le patch parent
 * @return PdSubpatch configuré ou nullptr si erreur
 */
unique_ptr<PdGuiObject> PdPatchParser::parseSubpatch(const vector<string>& tokens, ofVec2f pos) {
    // Vérifier le format minimal : #X restore x y pd subpatch_name
    if(tokens.size() < 5) {
        ofLogWarning("PdPatchParser") << "Subpatch line too short: " << tokens.size() << " tokens";
        return nullptr;
    }
    
    // Extraire le nom du subpatch (après "pd")
    string subpatchName;
    if(tokens[3] == "pd" && tokens.size() > 4) {
        subpatchName = tokens[4];
    } else if(tokens[4] == "pd" && tokens.size() > 5) {
        subpatchName = tokens[5];
    } else {
        ofLogWarning("PdPatchParser") << "Invalid subpatch format, missing 'pd' keyword";
        return nullptr;
    }
    
    // Construction du chemin vers le fichier subpatch
    // Convention : les subpatches sont dans le même répertoire que le patch principal
    string subpatchPath = subpatchName + ".pd";
    
    // Symboles send/receive par défaut basés sur le nom du subpatch
    string sendSymbol = subpatchName + "_send";
    string receiveSymbol = subpatchName + "_receive";
    
    // Taille par défaut pour le subpatch (peut être étendue plus tard)
    ofVec2f defaultSize(100, 100);
    
    try {
        // Créer l'objet PdSubpatch
        auto subpatch = make_unique<PdSubpatch>(
            pos,
            defaultSize,
            sendSymbol,
            receiveSymbol,
            subpatchPath,
            pos.x, // Utiliser la position comme offset de base
            pos.y
        );
        
        ofLogNotice("PdPatchParser") << "Created subpatch: " << subpatchName 
                                     << " at (" << pos.x << ", " << pos.y << ")";
        
        return subpatch;
        
    } catch(const exception& e) {
        ofLogError("PdPatchParser") << "Failed to create subpatch " << subpatchName 
                                    << ": " << e.what();
        return nullptr;
    }
}

vector<string> PdPatchParser::splitString(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    
    while(getline(ss, token, delimiter)) {
        if(!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}
