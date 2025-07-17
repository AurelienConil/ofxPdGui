#pragma once

#include "PdGuiObject.h"

class PdCanvas : public PdGuiObject {
public:
    // Constructeur
    PdCanvas(ofVec2f position, ofVec2f size,
             const string& label = "",
             ofColor backgroundColor = ofColor(224, 224, 224),
             ofColor textColor = ofColor(0, 0, 0));
    
    // Méthodes virtuelles héritées
    virtual void update() override;
    virtual void draw() override;
    
    // Surcharger les événements souris pour qu'ils ne soient PAS interceptés
    virtual bool onMousePressed(ofMouseEventArgs& args) override;
    virtual bool onMouseDragged(ofMouseEventArgs& args) override;
    virtual bool onMouseReleased(ofMouseEventArgs& args) override;
    virtual bool onMouseMoved(ofMouseEventArgs& args) override;
    
    // Configuration spécifique au canvas
    void setColors(ofColor backgroundColor, ofColor textColor);
    void setLabel(const string& label);
    void setLabelStyle(int fontSize = 12);
    
protected:
    // Propriétés spécifiques
    string canvasLabel;
    ofColor backgroundColor;
    ofColor textColor;  // Renommé de borderColor
    int labelFontSize;
    
    // Dessin spécialisé
    void drawCanvasBackground();
    void drawCanvasLabel();
    
    // Surcharger les méthodes de dessin de base
    virtual void drawBackground() override;
    virtual void drawBorder() override;  // Ne fait rien
    virtual void drawLabel() override;
};
