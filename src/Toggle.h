//
//  toogle.h
//  pd-gui
//
//  Created by Aurélien Conil on 15/07/2025.
//

// PdToggle.h
#pragma once

#include "PdGuiObject.h"

class PdToggle : public PdGuiObject {
public:
    // Constructeur
    PdToggle(ofVec2f position, ofVec2f size,
             const string& sendSymbol, const string& receiveSymbol);
    
    // Méthodes virtuelles obligatoires
    void update() override;
    void draw() override;
    
    // Gestion des événements souris (surcharge)
    bool onMousePressed(ofMouseEventArgs& args) override;
    bool onMouseReleased(ofMouseEventArgs& args) override;
    
    // Méthodes spécifiques au toggle
    void toggle();
    bool isOn() const { return currentValue > 0.5f; }
    void setOn(bool on) { setValue(on ? 1.0f : 0.0f); }
    
private:
    // Couleurs fixes pour le toggle
    static const ofColor TOGGLE_BG_COLOR;
    static const ofColor TOGGLE_BORDER_COLOR;
    static const ofColor TOGGLE_ON_COLOR;
    static const ofColor TOGGLE_OFF_COLOR;
    static const ofColor TOGGLE_HOVER_COLOR;
    
    // Méthodes de dessin privées
    void drawToggleState();
    void drawToggleBorder();
};
