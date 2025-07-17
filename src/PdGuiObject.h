
// PdGuiObject.h
#pragma once

#include "ofMain.h"
#include <functional>

enum class GuiType {
    HORIZONTAL_SLIDER,
    VERTICAL_SLIDER,
    TOGGLE,
    BANG,
    NUMBER_BOX,
    UNKNOWN
};

struct GuiUpdateRegion {
    ofRectangle rect;
    bool needsUpdate;
    
    GuiUpdateRegion() : needsUpdate(false) {}
    GuiUpdateRegion(ofRectangle r) : rect(r), needsUpdate(true) {}
};

class PdGuiObject {
public:
    // Constructeur
    PdGuiObject(GuiType type, ofVec2f position, ofVec2f size,
                const string& sendSymbol, const string& receiveSymbol);
    
    // Destructeur virtuel
    virtual ~PdGuiObject() = default;
    
    // Méthodes principales
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void drawToFbo(ofFbo& fbo) final;
    
    // Gestion des événements souris
    virtual bool onMousePressed(ofMouseEventArgs& args);
    virtual bool onMouseDragged(ofMouseEventArgs& args);
    virtual bool onMouseReleased(ofMouseEventArgs& args);
    virtual bool onMouseMoved(ofMouseEventArgs& args);
    
    // Gestion des valeurs
    virtual void setValue(float value);
    virtual float getValue() const { return currentValue; }
    virtual void setValueRange(float min, float max);
    
    // Gestion des mises à jour
    void markForUpdate();
    void markForUpdate(ofRectangle region);
    bool needsUpdate() const { return updateRegion.needsUpdate; }
    ofRectangle getUpdateRegion() const { return updateRegion.rect; }
    void clearUpdateFlag() { updateRegion.needsUpdate = false; }
    
    // Accesseurs
    GuiType getType() const { return type; }
    ofVec2f getPosition() const { return position; }
    ofVec2f getSize() const { return size; }
    ofRectangle getBounds() const { return ofRectangle(position.x, position.y, size.x, size.y); }
    
    string getSendSymbol() const { return sendSymbol; }
    string getReceiveSymbol() const { return receiveSymbol; }
    
    bool isVisible() const { return visible; }
    void setVisible(bool v) { visible = v; if(v) markForUpdate(); }
    
    bool isEnabled() const { return enabled; }
    void setEnabled(bool e) { enabled = e; markForUpdate(); }
    
    // Hit testing
    bool isPointInside(ofVec2f point) const;
    bool isPointInside(float x, float y) const;
    
    // Callbacks pour la communication avec Pure Data
    std::function<void(const string&, float)> onSendToPd;
    std::function<void(const string&, const string&)> onSendToPdString;
    
protected:
    // Propriétés de base
    GuiType type;
    ofVec2f position;
    ofVec2f size;
    string sendSymbol;
    string receiveSymbol;
    
    // Valeurs
    float currentValue;
    float minValue;
    float maxValue;
    
    // États
    bool visible;
    bool enabled;
    bool mouseOver;
    bool mousePressed;
    bool isDragging;
    
    // Gestion des mises à jour
    GuiUpdateRegion updateRegion;
    
    // Positions souris
    ofVec2f lastMousePos;
    ofVec2f mousePressPos;
    
    // Méthodes utilitaires protégées
    void sendToPd(float value);
    void sendToPd(const string& message);
    ofVec2f globalToLocal(ofVec2f globalPos) const;
    ofVec2f localToGlobal(ofVec2f localPos) const;
    
    // Dessin de base
    virtual void drawBackground();
    virtual void drawBorder();
    virtual void drawLabel();
    
    // Constantes de style
    static const ofColor DEFAULT_BG_COLOR;
    static const ofColor DEFAULT_FG_COLOR;
    static const ofColor DEFAULT_BORDER_COLOR;
    static const ofColor HOVER_COLOR;
    static const ofColor PRESSED_COLOR;
    
private:
    // Méthodes privées
    void updateMouseState(ofVec2f mousePos);
};
