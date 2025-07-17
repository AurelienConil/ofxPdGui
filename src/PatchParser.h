/**
 * @file PatchParser.h
 * @brief Système de parsing et décodage des fichiers Pure Data (.pd)
 * 
 * RÔLE DANS L'ARCHITECTURE :
 * Le PatchParser est le cœur du système de décodage de ofxPdGui. Il constitue
 * la première partie de l'architecture : PARSER/DECODER.
 * 
 * RESPONSABILITÉS :
 * - Lecture et analyse des fichiers .pd (format texte Pure Data)
 * - Extraction des objets GUI et de leurs paramètres 
 * - Conversion des données Pure Data vers objets C++ utilisables
 * - Support des objets GUI standards : bang, toggle, canvas, number, slider
 * 
 * FONCTIONNEMENT :
 * 1. Lecture ligne par ligne du fichier .pd
 * 2. Identification du type d'objet (#X obj x y type params...)
 * 3. Parsing des paramètres spécifiques à chaque type
 * 4. Création des objets C++ correspondants (PdToggle, PdBang, etc.)
 * 5. Retour d'une collection d'objets prêts pour le rendu
 * 
 * TYPES D'OBJETS SUPPORTÉS :
 * - hsl : Slider horizontal (PdSlider)
 * - vsl : Slider vertical (PdSlider)  
 * - tgl : Toggle/checkbox (PdToggle)
 * - bng : Bang/bouton (PdBang)
 * - cnv : Canvas/label (PdCanvas)
 * - floatatom : Boîte de nombre (PdNumberBox)
 * 
 * ROADMAP :
 * - Support des sous-patches (subpatches)
 * - Support des externals avec GUI
 * - Gestion des connexions entre objets
 * - Cache de parsing pour l'optimisation
 * 
 * @author Aurélien Conil
 * @date 2025
 */

//
//  PatchParser.h
//  pd-gui
//
//  Created by Aurélien Conil on 15/07/2025.
//

#pragma once

#include "ofMain.h"
#include "PdGuiObject.h"
#include "Toggle.h"
#include "Bang.h"
#include "Slider.h"
#include "NumberBox.h"
#include "Canvas.h"
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>

/**
 * @class PdPatchParser
 * @brief Parser principal pour les fichiers Pure Data
 * 
 * Cette classe implémente le système de décodage des fichiers .pd.
 * Elle convertit la syntaxe textuelle Pure Data en objets C++ utilisables
 * par le système de rendu.
 * 
 * ARCHITECTURE DE PARSING :
 * - parseFile() : Point d'entrée principal
 * - parseLine() : Analyse d'une ligne et dispatch vers les parsers spécialisés
 * - parseXXX() : Parsers spécialisés pour chaque type d'objet GUI
 * 
 * FORMAT PURE DATA :
 * Les fichiers .pd utilisent une syntaxe de type :
 * #X obj x y type param1 param2 ... paramN;
 * 
 * Exemple :
 * #X obj 100 50 tgl 40 0 toggle_send toggle_receive empty 0 -10 0 12 #fcfcfc #000000 #000000 0 1;
 */
class PdPatchParser {
public:
    // === MÉTHODE PRINCIPALE ===
    /// Parse un fichier .pd complet et retourne tous les objets GUI trouvés
    /// @param filename Chemin vers le fichier .pd (relatif à bin/data/)
    /// @return Collection d'objets GUI prêts pour le rendu
    std::vector<std::unique_ptr<PdGuiObject>> parseFile(const std::string& filename);
    
private:
    // === PARSERS SPÉCIALISÉS ===
    /// Parse une ligne individuelle du fichier .pd
    std::unique_ptr<PdGuiObject> parseLine(const std::string& line);
    
    /// Parse un slider horizontal : #X obj x y hsl width height min max ...
    std::unique_ptr<PdGuiObject> parseHorizontalSlider(const std::vector<std::string>& tokens, ofVec2f pos);
    
    /// Parse un slider vertical : #X obj x y vsl width height min max ...
    std::unique_ptr<PdGuiObject> parseVerticalSlider(const std::vector<std::string>& tokens, ofVec2f pos);
    
    /// Parse un toggle : #X obj x y tgl size nonzero send receive ...
    std::unique_ptr<PdGuiObject> parseToggle(const std::vector<std::string>& tokens, ofVec2f pos);
    
    /// Parse un bang : #X obj x y bng size hold send receive ...
    std::unique_ptr<PdGuiObject> parseBang(const std::vector<std::string>& tokens, ofVec2f pos);
    
    /// Parse un canvas/label : #X obj x y cnv size width height send receive label ...
    std::unique_ptr<PdGuiObject> parseCanvas(const vector<string>& tokens, ofVec2f pos);
    
    /// Parse une boîte de nombre : #X floatatom x y width min max ...
    std::unique_ptr<PdGuiObject> parseNumberBox(const std::vector<std::string>& tokens, ofVec2f pos);
    
    // === UTILITAIRES ===
    /// Divise une chaîne selon un délimiteur
    std::vector<std::string> splitString(const std::string& str, char delimiter);
    
    /// Convertit une couleur hexadécimale Pure Data en ofColor
    ofColor parseHexColor(const string& hexStr);
};
