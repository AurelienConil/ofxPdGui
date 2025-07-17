#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(60);
    ofBackground(50);
    ofSetWindowTitle("Pure Data Toggle Test");
    
    // Créer une liste de toggles
    //createToggles();
    
    PdPatchParser parser;
    guiObjects = parser.parseFile("patch.pd");
    
    
    // Configurer les callbacks pour tous les objets
    setupCallbacks();
    
    // Créer le FBO pour le rendu optimisé
    setupFbo();
    
    ofLogNotice("ofApp") << "Created " << guiObjects.size() << " toggles";
}

void ofApp::update() {
    // Mettre à jour tous les objets GUI
    for (auto& obj : guiObjects) {
        obj->update();
    }
    
    // Simulation : changer automatiquement quelques toggles
    //simulateAutomaticChanges();
}

void ofApp::draw() {
    // Dessiner le titre
    ofSetColor(255);
    ofDrawBitmapString("Pure Data Toggle Test - Click on toggles", 20, 30);
    ofDrawBitmapString("Total toggles: " + ofToString(guiObjects.size()), 20, 50);
    ofDrawBitmapString("Active toggles: " + ofToString(countActiveToggles()), 20, 70);
    
    // Dessiner tous les objets GUI
    drawGuiObjects();
    
    // Afficher les informations de debug
    drawDebugInfo();
}

void ofApp::mousePressed(int x, int y, int button) {
    ofMouseEventArgs args;
    args.x = x;
    args.y = y;
    args.button = button;
    
    // Propager l'événement à tous les objets
    for (auto& obj : guiObjects) {
        if (obj->onMousePressed(args)) {
            ofLogNotice("ofApp") << "Toggle clicked: " << obj->getSendSymbol();
            break; // Arrêter après le premier objet qui gère l'événement
        }
    }
}

void ofApp::mouseDragged(int x, int y, int button) {
    ofMouseEventArgs args;
    args.x = x;
    args.y = y;
    args.button = button;
    
    for (auto& obj : guiObjects) {
        obj->onMouseDragged(args);
    }
}

void ofApp::mouseReleased(int x, int y, int button) {
    ofMouseEventArgs args;
    args.x = x;
    args.y = y;
    args.button = button;
    
    for (auto& obj : guiObjects) {
        obj->onMouseReleased(args);
    }
}

void ofApp::mouseMoved(int x, int y) {
    ofMouseEventArgs args;
    args.x = x;
    args.y = y;
    
    for (auto& obj : guiObjects) {
        obj->onMouseMoved(args);
    }
}

void ofApp::keyPressed(int key) {
    if (key == 'r') {
        // Reset tous les toggles
        for (auto& obj : guiObjects) {
            obj->setValue(0.0f);
        }
        ofLogNotice("ofApp") << "All toggles reset";
    }
    else if (key == 'a') {
        // Activer tous les toggles
        for (auto& obj : guiObjects) {
            obj->setValue(1.0f);
        }
        ofLogNotice("ofApp") << "All toggles activated";
    }
    else if (key == 't') {
        // Toggle aléatoire
        if (!guiObjects.empty()) {
            int randomIndex = ofRandom(guiObjects.size());
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

void ofApp::setupCallbacks() {
    for (auto& obj : guiObjects) {
        // Callback pour l'envoi vers Pure Data
        obj->onSendToPd = [this](const string& symbol, float value) {
            ofLogNotice("PD Send") << symbol << " = " << value;
            // Ici on appellerait ofxPd::sendFloat(symbol, value);
        };
        
        // Callback pour l'envoi de strings vers Pure Data
        obj->onSendToPdString = [this](const string& symbol, const string& message) {
            ofLogNotice("PD Send String") << symbol << " = " << message;
            // Ici on appellerait ofxPd::sendSymbol(symbol, message);
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
