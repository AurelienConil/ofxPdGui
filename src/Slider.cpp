//
//  Slider.cpp
//  pd-gui
//
//  Created by Aurélien Conil on 16/07/2025.
//

#include "Slider.h"

PdSlider::PdSlider(GuiType type, ofVec2f position, ofVec2f size,
                   const string& sendSymbol, const string& receiveSymbol,
                   float min, float max, float initialValue)
    : PdGuiObject(type, position, size, sendSymbol, receiveSymbol)
    , knobSize(8.0f)
    , showValue(true)
    , isHorizontal(type == GuiType::HORIZONTAL_SLIDER)
    , dragOffset(0, 0)
    , isDraggingKnob(false)
{
    // Configurer la plage de valeurs
    setValueRange(min, max);
    setValue(initialValue);
}

void PdSlider::update() {
    // Mise à jour logique si nécessaire
    // Pour l'instant, rien de spécial à faire ici
}

void PdSlider::draw() {
    ofPushStyle();
    
    // Dessiner le fond
    drawBackground();
    
    // Dessiner la piste du slider
    drawTrack();
    
    // Dessiner le bouton/curseur
    drawKnob();
    
    // Dessiner la valeur si activé
    if (showValue) {
        drawValueText();
    }
    
    // Dessiner la bordure
    drawBorder();
    
    // Dessiner les labels de symboles
    drawLabel();
    
    ofPopStyle();
}

bool PdSlider::onMousePressed(ofMouseEventArgs& args) {
    if (!visible || !enabled) return false;
    
    ofVec2f mousePos(args.x, args.y);
    ofVec2f localMousePos = globalToLocal(mousePos);
    
    if (isPointInside(mousePos)) {
        mousePressed = true;
        mousePressPos = mousePos;
        lastMousePos = mousePos;
        
        // Vérifier si on clique sur le knob
        ofRectangle knobBounds = getKnobBounds();
        if (knobBounds.inside(localMousePos)) {
            isDraggingKnob = true;
            ofVec2f knobCenter = getKnobPosition();
            dragOffset = localMousePos - knobCenter;
        } else {
            // Clic sur la piste : déplacer le knob à cette position
            isDraggingKnob = true;
            dragOffset = ofVec2f(0, 0);
            float newValue = positionToValue(localMousePos);
            setSliderValue(newValue);
        }
        
        markForUpdate();
        return true;
    }
    
    return false;
}

bool PdSlider::onMouseDragged(ofMouseEventArgs& args) {
    if (!visible || !enabled || !mousePressed || !isDraggingKnob) return false;
    
    ofVec2f mousePos(args.x, args.y);
    ofVec2f localMousePos = globalToLocal(mousePos);
    
    // Calculer la nouvelle valeur basée sur la position de la souris
    ofVec2f adjustedPos = localMousePos - dragOffset;
    float newValue = positionToValue(adjustedPos);
    
    setSliderValue(newValue);
    lastMousePos = mousePos;
    
    return true;
}

bool PdSlider::onMouseReleased(ofMouseEventArgs& args) {
    bool wasPressed = mousePressed;
    bool result = PdGuiObject::onMouseReleased(args);
    
    if (wasPressed) {
        isDraggingKnob = false;
        dragOffset = ofVec2f(0, 0);
    }
    
    return result;
}

void PdSlider::setSliderValue(float value) {
    float oldValue = currentValue;
    setValue(value);
    
    // Envoyer la valeur à Pure Data si elle a changé
    if (abs(currentValue - oldValue) > 0.001f) {
        sendToPd(currentValue);
    }
}

float PdSlider::getSliderValue() const {
    return currentValue;
}

void PdSlider::setSliderRange(float min, float max) {
    setValueRange(min, max);
}

void PdSlider::setSliderStyle(float knobSize, bool showValue) {
    this->knobSize = knobSize;
    this->showValue = showValue;
    markForUpdate();
}

ofVec2f PdSlider::getKnobPosition() const {
    ofRectangle track = getTrackBounds();
    float normalizedValue = (currentValue - minValue) / (maxValue - minValue);
    
    if (isHorizontal) {
        float x = track.x + normalizedValue * track.width;
        float y = track.y + track.height * 0.5f;
        return ofVec2f(x, y);
    } else {
        float x = track.x + track.width * 0.5f;
        float y = track.y + track.height - (normalizedValue * track.height);
        return ofVec2f(x, y);
    }
}

ofRectangle PdSlider::getKnobBounds() const {
    ofVec2f knobPos = getKnobPosition();
    return ofRectangle(knobPos.x - knobSize * 0.5f,
                      knobPos.y - knobSize * 0.5f,
                      knobSize, knobSize);
}

ofRectangle PdSlider::getTrackBounds() const {
    float margin = knobSize * 0.5f + 2.0f;
    
    if (isHorizontal) {
        return ofRectangle(margin, size.y * 0.4f,
                          size.x - 2 * margin, size.y * 0.2f);
    } else {
        return ofRectangle(size.x * 0.4f, margin,
                          size.x * 0.2f, size.y - 2 * margin);
    }
}

float PdSlider::positionToValue(ofVec2f localPos) const {
    ofRectangle track = getTrackBounds();
    float normalizedValue;
    
    if (isHorizontal) {
        normalizedValue = (localPos.x - track.x) / track.width;
    } else {
        normalizedValue = 1.0f - ((localPos.y - track.y) / track.height);
    }
    
    normalizedValue = ofClamp(normalizedValue, 0.0f, 1.0f);
    return minValue + normalizedValue * (maxValue - minValue);
}

ofVec2f PdSlider::valueToPosition() const {
    return getKnobPosition();
}

void PdSlider::drawTrack() {
    ofRectangle track = getTrackBounds();
    
    // Dessiner la piste de fond
    ofSetColor(DEFAULT_BG_COLOR.getLerped(ofColor::black, 0.1f));
    ofDrawRectangle(track);
    
    // Dessiner la bordure de la piste
    ofSetColor(DEFAULT_BORDER_COLOR);
    ofNoFill();
    ofDrawRectangle(track);
    ofFill();
}

void PdSlider::drawKnob() {
    ofVec2f knobPos = getKnobPosition();
    
    // Couleur du knob selon l'état
    ofColor knobColor = DEFAULT_FG_COLOR;
    if (mousePressed && isDraggingKnob) {
        knobColor = PRESSED_COLOR;
    } else if (mouseOver) {
        knobColor = HOVER_COLOR;
    }
    
    if (!enabled) {
        knobColor = knobColor * 0.5f;
    }
    
    // Dessiner le knob
    ofSetColor(knobColor);
    ofDrawCircle(knobPos.x, knobPos.y, knobSize * 0.5f);
    
    // Bordure du knob
    ofSetColor(DEFAULT_BORDER_COLOR);
    ofNoFill();
    ofDrawCircle(knobPos.x, knobPos.y, knobSize * 0.5f);
    ofFill();
}

void PdSlider::drawValueText() {
    if (!showValue) return;
    
    ofSetColor(DEFAULT_FG_COLOR);
    
    // Formater la valeur
    string valueStr = ofToString(currentValue, 1);
    
    // Position du texte
    float textX, textY;
    if (isHorizontal) {
        textX = size.x * 0.5f - valueStr.length() * 3; // Approximation de centrage
        textY = size.y * 0.7f;
    } else {
        textX = size.x * 0.7f;
        textY = size.y * 0.5f;
    }
    
    ofDrawBitmapString(valueStr, textX, textY);
}
