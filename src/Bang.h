//
//  Bang.h
//  pd-gui
//
//  Created by Aurélien Conil on 15/07/2025.
//

#pragma once

#include "PdGuiObject.h"

class PdBang : public PdGuiObject {
public:
    // Couleurs fixes pour le bang
    static const ofColor BANG_BG_COLOR;
    static const ofColor BANG_BORDER_COLOR;
    static const ofColor BANG_CIRCLE_COLOR;
    static const ofColor BANG_HOVER_COLOR;
    
    // Constructeur
    PdBang(ofVec2f position, ofVec2f size,
           const string& sendSymbol, const string& receiveSymbol = "");
    
    // Méthodes virtuelles de PdGuiObject
    void update() override;
    void draw() override;
    
    // Gestion des événements souris
    bool onMousePressed(ofMouseEventArgs& args) override;
    bool onMouseReleased(ofMouseEventArgs& args) override;
    
    // Méthodes spécifiques au bang
    void trigger(); // Déclenche le bang
    bool isTriggered() const { return triggered; }
    
private:
    // État du bang
    bool triggered;
    float triggerTime;
    static const float TRIGGER_DURATION; // 300ms
    
    // Méthodes de dessin
    void drawBangState();
    void drawBangBorder();
    void drawCircle();
};
