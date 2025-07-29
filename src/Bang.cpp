//
//  Bang.cpp
//  pd-gui
//
//  Created by Aurélien Conil on 15/07/2025.
//

#include "Bang.h"
#include "ofApp.h" // Pour accéder aux méthodes statiques du mode

// Couleurs fixes pour le bang
const ofColor PdBang::BANG_BG_COLOR = ofColor(240, 240, 240);
const ofColor PdBang::BANG_BORDER_COLOR = ofColor(100, 100, 100);
const ofColor PdBang::BANG_CIRCLE_COLOR = ofColor(200, 50, 50);
const ofColor PdBang::BANG_HOVER_COLOR = ofColor(250, 250, 250);

// Durée d'affichage du cercle (300ms)
const float PdBang::TRIGGER_DURATION = 300.0f;

PdBang::PdBang(ofVec2f position, ofVec2f size,
               const string& sendSymbol, const string& receiveSymbol)
    : PdGuiObject(GuiType::BANG, position, size, sendSymbol, receiveSymbol)
{
    // Initialiser les valeurs spécifiques au bang
    minValue = 0.0f;
    maxValue = 1.0f;
    currentValue = 0.0f;
    triggered = false;
    triggerTime = 0.0f;
    
    // Initialiser les variables de déplacement
    isDraggingObject = false;
    dragOffset = ofVec2f(0, 0);
}

void PdBang::update() {
    // Vérifier si le bang est encore actif
    if (triggered) {
        float currentTime = ofGetElapsedTimeMillis();
        if (currentTime - triggerTime >= TRIGGER_DURATION) {
            triggered = false;
        }
    }
}

void PdBang::draw() {
    // Dessiner le fond
    drawBangState();
    
    // Dessiner le cercle si le bang est déclenché
    drawCircle();
    
    // Dessiner la bordure
    drawBangBorder();
    
    // Dessiner les labels send/receive si nécessaire
    //drawLabel();
}

bool PdBang::onMousePressed(ofMouseEventArgs& args) {
    // Appeler la méthode de base pour la gestion générale
    bool handled = PdGuiObject::onMousePressed(args);
    
    if (handled) {
        if (ofApp::isGlobalEditMode()) {
            // MODE ÉDITION : Préparer le déplacement de l'objet
            isDraggingObject = true;
            ofVec2f mousePos(args.x, args.y);
            dragOffset = mousePos - position;
            ofLogNotice("PdBang") << "Mode édition : début du drag pour " << sendSymbol;
        } else {
            // MODE UTILISATION : Déclencher le bang normalement
            trigger();
            sendToPd(1.0f);
            ofLogNotice("PdBang") << "Mode utilisation : bang déclenché pour " << sendSymbol;
        }
    }
    
    return handled;
}

bool PdBang::onMouseDragged(ofMouseEventArgs& args) {
    // Appeler la méthode de base
    bool handled = PdGuiObject::onMouseDragged(args);
    
    if (ofApp::isGlobalEditMode() && isDraggingObject) {
        // MODE ÉDITION : Déplacer l'objet
        ofVec2f mousePos(args.x, args.y);
        ofVec2f newPosition = mousePos - dragOffset;
        setPosition(newPosition);
        ofLogVerbose("PdBang") << "Déplacement vers (" << newPosition.x << ", " << newPosition.y << ")";
        return true;
    }
    
    return handled;
}

bool PdBang::onMouseReleased(ofMouseEventArgs& args) {
    // Finaliser le déplacement en mode édition
    if (ofApp::isGlobalEditMode() && isDraggingObject) {
        isDraggingObject = false;
        ofLogNotice("PdBang") << "Mode édition : fin du drag pour " << sendSymbol 
                              << " à la position (" << position.x << ", " << position.y << ")";
    }
    
    // Appeler la méthode de base
    return PdGuiObject::onMouseReleased(args);
}

void PdBang::trigger() {
    // Activer le bang et enregistrer le temps
    triggered = true;
    triggerTime = ofGetElapsedTimeMillis();
}

void PdBang::drawBangState() {
    // Couleur de fond
    ofColor bgColor = BANG_BG_COLOR;
    
    // Adapter la couleur selon le mode
    if (ofApp::isGlobalEditMode()) {
        // Mode édition : arrière-plan légèrement teinté de rouge
        bgColor = bgColor.getLerped(ofColor::red, 0.1f);
    }
    
    // Modifier la couleur selon l'état de la souris
    if (mousePressed) {
        bgColor = bgColor * 0.8f; // Plus sombre quand pressé
    } else if (mouseOver) {
        bgColor = bgColor.getLerped(BANG_HOVER_COLOR, 0.3f);
    }
    
    // Désactiver si nécessaire
    if (!enabled) {
        bgColor = bgColor * 0.5f;
    }
    
    // Dessiner le rectangle de fond
    ofSetColor(bgColor);
    ofFill();
    ofDrawRectangle(0, 0, size.x, size.y);
}

void PdBang::drawBangBorder() {
    // Couleur de bordure selon l'état
    ofColor borderColor = BANG_BORDER_COLOR;
    
    if (mouseOver) {
        borderColor = borderColor * 0.7f; // Plus sombre au survol
    }
    
    if (!enabled) {
        borderColor = borderColor * 0.5f;
    }
    
    // Dessiner la bordure
    ofSetColor(borderColor);
    ofNoFill();
    ofSetLineWidth(1.0f);
    ofDrawRectangle(0, 0, size.x, size.y);
    
    // Restaurer le fill
    ofFill();
}

void PdBang::drawCircle() {
    // Couleur du cercle
    ofColor circleColor = BANG_CIRCLE_COLOR;
    
    // Calculer la taille du cercle (circonscrit dans le carré)
    float radius = min(size.x, size.y) / 2.0f - 2.0f; // Petite marge
    float centerX = size.x / 2.0f;
    float centerY = size.y / 2.0f;
    
    // Dessiner le cercle
    ofSetColor(circleColor);
    if(triggered)ofFill();
    else ofNoFill();
    ofDrawCircle(centerX, centerY, radius);
}
