//
//  PdGuiObject.cpp
//  pd-gui
//
//  Created by Aurélien Conil on 15/07/2025.
//

#include "PdGuiObject.h"

// Constantes de style
const ofColor PdGuiObject::DEFAULT_BG_COLOR = ofColor(220, 220, 220);
const ofColor PdGuiObject::DEFAULT_FG_COLOR = ofColor(50, 50, 50);
const ofColor PdGuiObject::DEFAULT_BORDER_COLOR = ofColor(100, 100, 100);
const ofColor PdGuiObject::HOVER_COLOR = ofColor(240, 240, 240);
const ofColor PdGuiObject::PRESSED_COLOR = ofColor(180, 180, 180);

PdGuiObject::PdGuiObject(GuiType type, ofVec2f position, ofVec2f size,
                         const string& sendSymbol, const string& receiveSymbol)
    : type(type)
    , position(position)
    , size(size)
    , sendSymbol(sendSymbol)
    , receiveSymbol(receiveSymbol)
    , currentValue(0.0f)
    , minValue(0.0f)
    , maxValue(127.0f)
    , visible(true)
    , enabled(true)
    , mouseOver(false)
    , mousePressed(false)
    , isDragging(false)
    , lastMousePos(0, 0)
    , mousePressPos(0, 0)
{
    // Initialiser la région de mise à jour avec les dimensions complètes
    updateRegion = GuiUpdateRegion(getBounds());
    
    // Callbacks par défaut (vides)
    onSendToPd = [](const string&, float) {};
    onSendToPdString = [](const string&, const string&) {};
}

void PdGuiObject::drawToFbo(ofFbo& fbo) {
    if (!visible || !needsUpdate()) return;
    
    // Sauvegarder l'état actuel
    ofPushMatrix();
    ofPushStyle();
    
    // Configurer le FBO
    fbo.begin();
    
    // Traduire vers la position de l'objet
    ofTranslate(position.x, position.y);
    
    // Dessiner l'objet
    draw();
    
    fbo.end();
    
    // Restaurer l'état
    ofPopStyle();
    ofPopMatrix();
    
    // Marquer comme mis à jour
    clearUpdateFlag();
}

bool PdGuiObject::onMousePressed(ofMouseEventArgs& args) {
    if (!visible || !enabled) return false;
    
    ofVec2f mousePos(args.x, args.y);
    
    if (isPointInside(mousePos)) {
        mousePressed = true;
        mousePressPos = mousePos;
        lastMousePos = mousePos;
        markForUpdate();
        return true;
    }
    
    return false;
}

bool PdGuiObject::onMouseDragged(ofMouseEventArgs& args) {
    if (!visible || !enabled || !mousePressed) return false;
    
    ofVec2f mousePos(args.x, args.y);
    
    // Vérifier si on est en train de draguer
    if (!isDragging) {
        float dragDistance = mousePressPos.distance(mousePos);
        if (dragDistance > 3.0f) { // Seuil de drag
            isDragging = true;
        }
    }
    
    if (isDragging) {
        lastMousePos = mousePos;
        markForUpdate();
        return true;
    }
    
    return false;
}

bool PdGuiObject::onMouseReleased(ofMouseEventArgs& args) {
    if (!visible || !enabled) return false;
    
    bool wasPressed = mousePressed;
    mousePressed = false;
    isDragging = false;
    
    if (wasPressed) {
        markForUpdate();
        return true;
    }
    
    return false;
}

bool PdGuiObject::onMouseMoved(ofMouseEventArgs& args) {
    if (!visible || !enabled) return false;
    
    ofVec2f mousePos(args.x, args.y);
    bool wasMouseOver = mouseOver;
    mouseOver = isPointInside(mousePos);
    
    if (wasMouseOver != mouseOver) {
        markForUpdate();
    }
    
    return mouseOver;
}

void PdGuiObject::setValue(float value) {
    float clampedValue = ofClamp(value, minValue, maxValue);
    
    if (abs(currentValue - clampedValue) > 0.001f) {
        currentValue = clampedValue;
        markForUpdate();
    }
}

void PdGuiObject::setValueRange(float min, float max) {
    minValue = min;
    maxValue = max;
    
    // Reclamper la valeur actuelle
    setValue(currentValue);
}

void PdGuiObject::markForUpdate() {
    updateRegion = GuiUpdateRegion(getBounds());
}

void PdGuiObject::markForUpdate(ofRectangle region) {
    updateRegion = GuiUpdateRegion(region);
}

bool PdGuiObject::isPointInside(ofVec2f point) const {
    return getBounds().inside(point);
}

bool PdGuiObject::isPointInside(float x, float y) const {
    return isPointInside(ofVec2f(x, y));
}

void PdGuiObject::sendToPd(float value) {
    if (!sendSymbol.empty() && sendSymbol != "empty") {
        onSendToPd(sendSymbol, value);
    }
}

void PdGuiObject::sendToPd(const string& message) {
    if (!sendSymbol.empty() && sendSymbol != "empty") {
        onSendToPdString(sendSymbol, message);
    }
}

ofVec2f PdGuiObject::globalToLocal(ofVec2f globalPos) const {
    return globalPos - position;
}

ofVec2f PdGuiObject::localToGlobal(ofVec2f localPos) const {
    return localPos + position;
}

void PdGuiObject::drawBackground() {
    ofColor bgColor = DEFAULT_BG_COLOR;
    
    if (mousePressed) {
        bgColor = PRESSED_COLOR;
    } else if (mouseOver) {
        bgColor = HOVER_COLOR;
    }
    
    if (!enabled) {
        bgColor = bgColor * 0.5f;
    }
    
    ofSetColor(bgColor);
    ofDrawRectangle(0, 0, size.x, size.y);
}

void PdGuiObject::drawBorder() {
    ofSetColor(DEFAULT_BORDER_COLOR);
    ofNoFill();
    ofDrawRectangle(0, 0, size.x, size.y);
    ofFill();
}

void PdGuiObject::drawLabel() {
    if (!sendSymbol.empty() && sendSymbol != "empty") {
        ofSetColor(DEFAULT_FG_COLOR);
        ofDrawBitmapString("S:" + sendSymbol, 2, size.y + 12);
    }
    
    if (!receiveSymbol.empty() && receiveSymbol != "empty") {
        ofSetColor(DEFAULT_FG_COLOR);
        ofDrawBitmapString("R:" + receiveSymbol, 2, size.y + 24);
    }
}

void PdGuiObject::updateMouseState(ofVec2f mousePos) {
    lastMousePos = mousePos;
}
