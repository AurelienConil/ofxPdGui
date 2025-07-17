//
//  Toogle.cpp
//  pd-gui
//
//  Created by Aurélien Conil on 15/07/2025.
//

#include "Toggle.h"

// Couleurs fixes pour le toggle
const ofColor PdToggle::TOGGLE_BG_COLOR = ofColor(240, 240, 240);
const ofColor PdToggle::TOGGLE_BORDER_COLOR = ofColor(100, 100, 100);
const ofColor PdToggle::TOGGLE_ON_COLOR = ofColor(200, 50, 50);
const ofColor PdToggle::TOGGLE_OFF_COLOR = ofColor(220, 220, 220);
const ofColor PdToggle::TOGGLE_HOVER_COLOR = ofColor(250, 250, 250);

PdToggle::PdToggle(ofVec2f position, ofVec2f size,
                   const string& sendSymbol, const string& receiveSymbol)
    : PdGuiObject(GuiType::TOGGLE, position, size, sendSymbol, receiveSymbol)
{
    // Initialiser les valeurs spécifiques au toggle
    minValue = 0.0f;
    maxValue = 1.0f;
    currentValue = 0.0f;
}

void PdToggle::update() {
    // Logique de mise à jour spécifique au toggle
    // Pour l'instant, rien de spécial à faire
    // Cette méthode peut être utilisée pour des animations futures
}

void PdToggle::draw() {
    // Dessiner le fond
    drawToggleState();
    
    // Dessiner la bordure
    drawToggleBorder();
    
    // Dessiner les labels send/receive
    //drawLabel();
}

bool PdToggle::onMousePressed(ofMouseEventArgs& args) {
    // Appeler la méthode de base pour la gestion générale
    bool handled = PdGuiObject::onMousePressed(args);
    
    if (handled) {
        // Le toggle change d'état immédiatement au clic
        toggle();
        
        // Envoyer la nouvelle valeur à Pure Data
        sendToPd(currentValue);
    }
    
    return handled;
}

bool PdToggle::onMouseReleased(ofMouseEventArgs& args) {
    // Appeler la méthode de base
    return PdGuiObject::onMouseReleased(args);
}

void PdToggle::toggle() {
    // Inverser l'état
    setOn(!isOn());
}

void PdToggle::drawToggleState() {
    // Déterminer la couleur de fond selon l'état
    ofColor bgColor;
    
    if (isOn()) {
        bgColor = TOGGLE_ON_COLOR;
    } else {
        bgColor = TOGGLE_OFF_COLOR;
    }
    
    // Modifier la couleur selon l'état de la souris
    if (mousePressed) {
        bgColor = bgColor * 0.8f; // Plus sombre quand pressé
    } else if (mouseOver) {
        bgColor = bgColor.getLerped(TOGGLE_HOVER_COLOR, 0.3f);
    }
    
    // Désactiver si nécessaire
    if (!enabled) {
        bgColor = bgColor * 0.5f;
    }
    
    // Dessiner le rectangle de fond
    ofSetColor(bgColor);
    ofFill();
    ofDrawRectangle(0, 0, size.x, size.y);
    
    // Si le toggle est ON, dessiner un rectangle intérieur plus sombre
    if (isOn()) {
        ofSetColor(TOGGLE_ON_COLOR * 0.7f);
        
        // Rectangle intérieur avec une petite marge
        float margin = 2.0f;
        ofDrawRectangle(margin, margin,
                       size.x - 2 * margin,
                       size.y - 2 * margin);
    }
}

void PdToggle::drawToggleBorder() {
    // Couleur de bordure selon l'état
    ofColor borderColor = TOGGLE_BORDER_COLOR;
    
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
