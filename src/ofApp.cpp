/**
 * @file ofApp.cpp
 * @brief Implémentation de la classe principale ofApp - Cœur du rendu et de l'interaction
 * 
 * Ce fichier contient l'implémentation du moteur de rendu principal de ofxPdGui.
 * Il coordonne le chargement des patches, la gestion des objets GUI et l'optimisation 
 * des performances via le système FBO.
 * 
 * FLUX D'EXÉCUTION :
 * 1. setup() : Charge le patch.pd et initialise le système FBO
 * 2. update() : Met à jour les objets GUI à chaque frame
 * 3. draw() : Effectue le rendu optimisé des objets
 * 4. Événements souris/clavier : Propagés vers les objets concernés
 * 
 * INTÉGRATION ofxPd (PRÉPARÉE) :
 * Les callbacks sont configurés mais utilisent actuellement des logs.
 * Pour l'intégration complète, remplacer les ofLogNotice par :
 * - ofxPd::sendFloat(symbol, value)
 * - ofxPd::sendSymbol(symbol, message)
 */

#include "ofApp.h"

/**
 * @brief Initialisation principale de l'application
 * 
 * ÉTAPES D'INITIALISATION :
 * 1. Configuration openFrameworks (framerate, background, titre)
 * 2. Parsing du fichier patch.pd via PdPatchParser
 * 3. Configuration des callbacks pour l'interface Pure Data
 * 4. Initialisation du système FBO pour l'optimisation
 * 
 * Le parser analyse automatiquement tous les objets GUI du fichier .pd
 * et les convertit en objets C++ utilisables (PdToggle, PdBang, etc.)
 */
void ofApp::setup() {
    ofSetFrameRate(60);
    ofBackground(50);
    ofSetWindowTitle("Pure Data Toggle Test");
    
    // === CHARGEMENT DU PATCH PURE DATA ===
    // Le parser convertit automatiquement les objets .pd en objets C++
    //createToggles(); // Méthode legacy pour les tests manuels
    
    PdPatchParser parser;
    guiObjects = parser.parseFile("patch.pd");  // Charge bin/data/patch.pd
    
    // === CONFIGURATION DE L'INTERFACE PURE DATA ===
    // Les callbacks sont prêts pour l'intégration ofxPd
    setupCallbacks();
    
    // === INITIALISATION DU SYSTÈME FBO ===
    // Prépare l'optimisation de rendu pour de meilleures performances
    setupFbo();
    
    ofLogNotice("ofApp") << "Created " << guiObjects.size() << " GUI objects from patch.pd";
}

/**
 * @brief Mise à jour à chaque frame
 * 
 * Met à jour tous les objets GUI chargés. Chaque objet peut avoir sa propre
 * logique de mise à jour (animations, états temporaires, etc.)
 * 
 * OPTIMISATION : Les objets marquent automatiquement leurs zones de mise à jour
 * pour le système FBO, évitant le rendu complet à chaque frame.
 */
void ofApp::update() {
    // Mettre à jour tous les objets GUI - propagation automatique des changements
    for (auto& obj : guiObjects) {
        obj->update();
    }
    
    // === FONCTIONNALITÉ DE TEST ===
    // Simulation automatique désactivée par défaut
    //simulateAutomaticChanges();
}

/**
 * @brief Rendu principal de l'application
 * 
 * ARCHITECTURE DE RENDU :
 * 1. Interface utilisateur : titre, compteurs, informations
 * 2. Objets GUI : rendu via drawGuiObjects() (direct) ou drawGuiObjectsToFbo() (optimisé)
 * 3. Informations de debug : contrôles et objets actifs
 * 
 * SYSTÈME FBO : Actuellement en rendu direct, le système FBO est préparé
 * pour l'optimisation future des performances sur de gros patches.
 */
void ofApp::draw() {
    // === INTERFACE UTILISATEUR ===
    ofSetColor(255);
    ofDrawBitmapString("Pure Data Patch Renderer - ofxPdGui", 20, 30);
    ofDrawBitmapString("Total objects: " + ofToString(guiObjects.size()), 20, 50);
    ofDrawBitmapString("Active toggles: " + ofToString(countActiveToggles()), 20, 70);
    
    // === RENDU DES OBJETS GUI ===
    // Méthode actuelle : rendu direct pour la simplicité
    drawGuiObjects();
    
    // Méthode future : rendu optimisé via FBO
    // drawGuiObjectsToFbo();
    
    // === INFORMATIONS DE DEBUG ===
    drawDebugInfo();
}

/**
 * @brief Gestion des clics souris - Activation des objets GUI
 * 
 * Propage l'événement vers tous les objets GUI pour test de collision.
 * Le premier objet qui gère l'événement arrête la propagation.
 * 
 * @param x, y Position du clic
 * @param button Bouton souris (0=gauche, 1=droite, 2=molette)
 */
void ofApp::mousePressed(int x, int y, int button) {
    ofMouseEventArgs args;
    args.x = x;
    args.y = y;
    args.button = button;
    
    // Propagation vers les objets GUI - le premier qui répond arrête la chaîne
    for (auto& obj : guiObjects) {
        if (obj->onMousePressed(args)) {
            ofLogNotice("ofApp") << "Object activated: " << obj->getSendSymbol();
            break; // Arrêter après le premier objet qui gère l'événement
        }
    }
}

/**
 * @brief Gestion du drag souris - Sliders et interactions continues
 * 
 * Propage l'événement vers tous les objets. Particulièrement important
 * pour les sliders et autres contrôles continus.
 */
void ofApp::mouseDragged(int x, int y, int button) {
    ofMouseEventArgs args;
    args.x = x;
    args.y = y;
    args.button = button;
    
    for (auto& obj : guiObjects) {
        obj->onMouseDragged(args);
    }
}

/**
 * @brief Gestion du relâchement souris - Fin des interactions
 * 
 * Finalise les interactions en cours (drag de sliders, etc.)
 */
void ofApp::mouseReleased(int x, int y, int button) {
    ofMouseEventArgs args;
    args.x = x;
    args.y = y;
    args.button = button;
    
    for (auto& obj : guiObjects) {
        obj->onMouseReleased(args);
    }
}

/**
 * @brief Gestion du mouvement souris - Hover et feedback visuel
 * 
 * Permet aux objets de donner un feedback visuel lors du survol
 */
void ofApp::mouseMoved(int x, int y) {
    ofMouseEventArgs args;
    args.x = x;
    args.y = y;
    
    for (auto& obj : guiObjects) {
        obj->onMouseMoved(args);
    }
}

/**
 * @brief Gestion des raccourcis clavier
 * 
 * RACCOURCIS DISPONIBLES :
 * - 'r' : Reset de tous les objets (valeur = 0)
 * - 'a' : Activation de tous les objets (valeur = 1) 
 * - 't' : Toggle aléatoire d'un objet
 * 
 * Ces raccourcis sont utiles pour les tests et la démo.
 */
void ofApp::keyPressed(int key) {
    if (key == 'r') {
        // Reset de tous les objets GUI
        for (auto& obj : guiObjects) {
            obj->setValue(0.0f);
        }
        ofLogNotice("ofApp") << "All objects reset";
    }
    else if (key == 'a') {
        // Activation de tous les objets GUI
        for (auto& obj : guiObjects) {
            obj->setValue(1.0f);
        }
        ofLogNotice("ofApp") << "All objects activated";
    }
    else if (key == 't') {
        // Toggle aléatoire d'un objet
        if (!guiObjects.empty()) {
            int randomIndex = ofRandom(guiObjects.size());
            // Note: Cette conversion fonctionne pour les toggles, à adapter pour autres types
            PdToggle* toggle = static_cast<PdToggle*>(guiObjects[randomIndex].get());
            toggle->toggle();
            ofLogNotice("ofApp") << "Random toggle: " << toggle->getSendSymbol();
        }
    }
}

void ofApp::createToggles() {
    // Créer 4 toggles avec différentes tailles
    float startX = 100.0f;
    float startY = 100.0f;
    float spacing = 80.0f;
    
    // Toggle 1 - Petit
    auto toggle1 = make_unique<PdToggle>(
        ofVec2f(startX, startY),
        ofVec2f(25, 25),
        "toggle_1_send",
        "toggle_1_receive"
    );
    guiObjects.push_back(move(toggle1));
    
    // Toggle 2 - Moyen
    auto toggle2 = make_unique<PdToggle>(
        ofVec2f(startX + spacing, startY),
        ofVec2f(40, 40),
        "toggle_2_send",
        "toggle_2_receive"
    );
    guiObjects.push_back(move(toggle2));
    
    // Toggle 3 - Grand
    auto toggle3 = make_unique<PdToggle>(
        ofVec2f(startX + spacing * 2, startY),
        ofVec2f(55, 55),
        "toggle_3_send",
        "toggle_3_receive"
    );
    guiObjects.push_back(move(toggle3));
    
    // Toggle 4 - Très grand
    auto toggle4 = make_unique<PdToggle>(
        ofVec2f(startX + spacing * 3, startY),
        ofVec2f(70, 70),
        "toggle_4_send",
        "toggle_4_receive"
    );
    guiObjects.push_back(move(toggle4));



    // Créer 4 bangs avec différentes tailles
    startX = 100.0f;
    startY = 220.0f;  // En dessous des toggles
    spacing = 80.0f;
    
    // Bang 1 - Petit
    auto bang1 = make_unique<PdBang>(
        ofVec2f(startX, startY),
        ofVec2f(25, 25),
        "bang_1_send",
        "bang_1_receive"
    );
    guiObjects.push_back(move(bang1));
    
    // Bang 2 - Moyen
    auto bang2 = make_unique<PdBang>(
        ofVec2f(startX + spacing, startY),
        ofVec2f(40, 40),
        "bang_2_send",
        "bang_2_receive"
    );
    guiObjects.push_back(move(bang2));
    
    // Bang 3 - Grand
    auto bang3 = make_unique<PdBang>(
        ofVec2f(startX + spacing * 2, startY),
        ofVec2f(55, 55),
        "bang_3_send",
        "bang_3_receive"
    );
    guiObjects.push_back(move(bang3));
    
    // Bang 4 - Très grand
    auto bang4 = make_unique<PdBang>(
        ofVec2f(startX + spacing * 3, startY),
        ofVec2f(70, 70),
        "bang_4_send",
        "bang_4_receive"
    );
    guiObjects.push_back(move(bang4));
}

/**
 * @brief Configuration du système de callbacks vers Pure Data
 * 
 * INTERFACE ofxPd PRÊTE :
 * Les callbacks sont configurés pour tous les objets GUI. Actuellement ils utilisent
 * des logs pour les tests, mais sont prêts pour l'intégration ofxPd complète.
 * 
 * POUR L'INTÉGRATION COMPLÈTE, REMPLACER :
 * - ofLogNotice par ofxPd::sendFloat(symbol, value)
 * - ofLogNotice par ofxPd::sendSymbol(symbol, message)
 * 
 * Cette architecture permet une intégration transparente avec le moteur Pure Data.
 */
void ofApp::setupCallbacks() {
    for (auto& obj : guiObjects) {
        // === CALLBACK POUR VALEURS NUMÉRIQUES ===
        // Sera appelé quand un objet GUI change de valeur
        obj->onSendToPd = [this](const string& symbol, float value) {
            ofLogNotice("PD Send") << symbol << " = " << value;
            // À INTÉGRER : ofxPd::sendFloat(symbol, value);
        };
        
        // === CALLBACK POUR MESSAGES TEXTE ===
        // Sera appelé pour les messages texte (bang, etc.)
        obj->onSendToPdString = [this](const string& symbol, const string& message) {
            ofLogNotice("PD Send String") << symbol << " = " << message;
            // À INTÉGRER : ofxPd::sendSymbol(symbol, message);
        };
    }
}

void ofApp::setupFbo() {
    guiFbo.allocate(ofGetWidth(), ofGetHeight());
    guiFbo.begin();
    ofClear(0, 0, 0, 0);
    guiFbo.end();
}

void ofApp::drawGuiObjects() {
    // Méthode 1: Dessin direct (plus simple pour le test)
    for (auto& obj : guiObjects) {
        if (obj->isVisible()) {
            ofPushMatrix();
            ofTranslate(obj->getPosition().x, obj->getPosition().y);
            obj->draw();
            ofPopMatrix();
        }
    }
    
    // Méthode 2: Dessin via FBO (pour l'optimisation future)
    // drawGuiObjectsToFbo();
}

void ofApp::drawGuiObjectsToFbo() {
    // Vérifier quels objets ont besoin d'être redessinés
    bool anyObjectNeedsUpdate = false;
    
    for (auto& obj : guiObjects) {
        if (obj->needsUpdate()) {
            anyObjectNeedsUpdate = true;
            break;
        }
    }
    
    // Si des objets ont changé ou si c'est la première fois
    if (anyObjectNeedsUpdate || fboNeedsUpdate) {
        guiFbo.begin();
        
        // Parcourir tous les objets et redessiner seulement ceux qui ont changé
        for (auto& obj : guiObjects) {
            if (obj->needsUpdate() && obj->isVisible()) {
                // Récupérer la zone de mise à jour spécifique
                ofRectangle updateRect = obj->getUpdateRegion();
                
                // Effacer la zone de l'objet GUI avec une petite marge
                float margin = 1.0f;
                ofEnableBlendMode(OF_BLENDMODE_ALPHA);
                ofSetColor(0, 0, 0, 0); // Transparent
                ofDrawRectangle(updateRect.x - margin, updateRect.y - margin,
                               updateRect.width + 2 * margin, updateRect.height + 2 * margin);
                
                // Restaurer le blending normal et redessiner l'objet
                ofEnableBlendMode(OF_BLENDMODE_ALPHA);
                ofSetColor(255);
                
                // Utiliser la méthode drawToFbo de l'objet
                obj->drawToFbo(guiFbo);
                
                // Marquer l'objet comme mis à jour
                obj->clearUpdateFlag();
            }
        }
        
        guiFbo.end();
        fboNeedsUpdate = false;
    }
    
    // Dessiner le FBO
    guiFbo.draw(0, 0);
}

void ofApp::simulateAutomaticChanges() {
    simulationTime += ofGetLastFrameTime();
    
    // Changer un toggle aléatoire toutes les 2 secondes
    if (simulationTime > 2.0f) {
        simulationTime = 0.0f;
        
        if (!guiObjects.empty() && ofRandom(1.0f) < 0.3f) { // 30% de chance
            int randomIndex = ofRandom(guiObjects.size());
            PdToggle* toggle = static_cast<PdToggle*>(guiObjects[randomIndex].get());
            toggle->toggle();
        }
    }
}

int ofApp::countActiveToggles() {
    int count = 0;
    for (auto& obj : guiObjects) {
        if (obj->getValue() > 0.5f) {
            count++;
        }
    }
    return count;
}

void ofApp::drawDebugInfo() {
    ofSetColor(255, 255, 0);
    ofDrawBitmapString("Controls:", 20, ofGetHeight() - 80);
    ofDrawBitmapString("'r' - Reset all toggles", 20, ofGetHeight() - 60);
    ofDrawBitmapString("'a' - Activate all toggles", 20, ofGetHeight() - 40);
    ofDrawBitmapString("'t' - Toggle random", 20, ofGetHeight() - 20);
    
    // Afficher les informations sur les objets qui ont le focus
    ofSetColor(200, 200, 255);
    int yPos = 300;
    for (auto& obj : guiObjects) {
        if (obj->getValue() > 0.5f) {
            ofDrawBitmapString("Active: " + obj->getSendSymbol(), 400, yPos);
            yPos += 15;
            if (yPos > ofGetHeight() - 50) break;
        }
    }
}
