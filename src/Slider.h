//
//  Slider.h
//  pd-gui
//
//  Created by Aurélien Conil on 16/07/2025.
//

#pragma once

#include "PdGuiObject.h"

class PdSlider : public PdGuiObject {
public:
    // Constructeur
    PdSlider(GuiType type, ofVec2f position, ofVec2f size,
             const string& sendSymbol, const string& receiveSymbol,
             float min = 0.0f, float max = 127.0f, float initialValue = 0.0f);
    
    // Méthodes virtuelles héritées
    virtual void update() override;
    virtual void draw() override;
    
    // Gestion spécifique des événements souris pour le slider
    virtual bool onMousePressed(ofMouseEventArgs& args) override;
    virtual bool onMouseDragged(ofMouseEventArgs& args) override;
    virtual bool onMouseReleased(ofMouseEventArgs& args) override;
    
    // Méthodes spécifiques au slider
    void setSliderValue(float value);
    float getSliderValue() const;
    
    // Configuration du slider
    void setSliderRange(float min, float max);
    void setSliderStyle(float knobSize = 8.0f, bool showValue = true);
    
protected:
    // Propriétés spécifiques au slider
    float knobSize;
    bool showValue;
    bool isHorizontal;
    
    // Calculs de position
    ofVec2f getKnobPosition() const;
    ofRectangle getKnobBounds() const;
    ofRectangle getTrackBounds() const;
    
    // Conversion valeur <-> position
    float positionToValue(ofVec2f mousePos) const;
    ofVec2f valueToPosition() const;
    
    // Dessin spécialisé
    void drawTrack();
    void drawKnob();
    void drawValueText();
    
private:
    // Variables pour le drag
    ofVec2f dragOffset;
    bool isDraggingKnob;
};
