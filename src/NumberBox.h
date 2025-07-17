//
//  NumberBox.h
//  pd-gui
//
//  Created by Aurélien Conil on 16/07/2025.
//

#pragma once

#include "PdGuiObject.h"

class PdNumberBox : public PdGuiObject {
public:
    // Constructeur
    PdNumberBox(ofVec2f position, ofVec2f size,
                const string& sendSymbol, const string& receiveSymbol,
                float min = -1000000.0f, float max = 1000000.0f,
                float initialValue = 0.0f, int precision = 2);
    
    // Méthodes virtuelles héritées
    virtual void update() override;
    virtual void draw() override;
    
    // Gestion spécifique des événements souris
    virtual bool onMousePressed(ofMouseEventArgs& args) override;
    virtual bool onMouseDragged(ofMouseEventArgs& args) override;
    virtual bool onMouseReleased(ofMouseEventArgs& args) override;
    virtual bool onMouseMoved(ofMouseEventArgs& args) override;
    
    // Méthodes spécifiques au number box
    void setNumberValue(float value);
    float getNumberValue() const;
    void setPrecision(int precision);
    int getPrecision() const { return displayPrecision; }
    
    // Configuration
    void setDragSensitivity(float sensitivity);
    void setValueRange(float min, float max);
    
protected:
    // Propriétés spécifiques
    int displayPrecision;
    float dragSensitivity;
    float dragStartValue;
    float dragStartY;
    bool isDraggingValue;
    
    // Formatage et affichage
    string formatValue() const;
    void drawNumberText();
    void drawBackground() override;
    
    // Calculs de drag
    float calculateDragDelta(float currentY) const;
    void updateValueFromDrag(float deltaY);
    
private:
    static const float DEFAULT_DRAG_SENSITIVITY;
    static const int DEFAULT_FONT_SIZE;
};
