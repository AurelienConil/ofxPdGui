//
//  NumberBox.cpp
//  pd-gui
//
//  Created by Aurélien Conil on 16/07/2025.
//

#include "NumberBox.h"

// Constantes
const float PdNumberBox::DEFAULT_DRAG_SENSITIVITY = 0.5f;
const int PdNumberBox::DEFAULT_FONT_SIZE = 12;

PdNumberBox::PdNumberBox(ofVec2f position, ofVec2f size,
                         const string& sendSymbol, const string& receiveSymbol,
                         float min, float max, float initialValue, int precision)
    : PdGuiObject(GuiType::NUMBER_BOX, position, size, sendSymbol, receiveSymbol)
    , displayPrecision(precision)
    , dragSensitivity(DEFAULT_DRAG_SENSITIVITY)
    , dragStartValue(0.0f)
    , dragStartY(0.0f)
    , isDraggingValue(false)
{
    setValueRange(min, max);
    setValue(initialValue);
}

void PdNumberBox::update() {
    // Mise à jour logique si nécessaire
}

void PdNumberBox::draw() {
    ofPushStyle();
    
    // Dessiner le fond
    drawBackground();
    
    // Dessiner le texte du nombre
    drawNumberText();
    
    // Dessiner la bordure
    drawBorder();
    
    // Dessiner les labels de symboles
    drawLabel();
    
    ofPopStyle();
}

bool PdNumberBox::onMousePressed(ofMouseEventArgs& args) {
    if (!visible || !enabled) return false;
    
    ofVec2f mousePos(args.x, args.y);
    
    if (isPointInside(mousePos)) {
        mousePressed = true;
        mousePressPos = mousePos;
        lastMousePos = mousePos;
        
        // Préparer pour le drag
        dragStartValue = currentValue;
        dragStartY = mousePos.y;
        isDraggingValue = false; // Sera activé lors du premier drag
        
        markForUpdate();
        return true;
    }
    
    return false;
}

bool PdNumberBox::onMouseDragged(ofMouseEventArgs& args) {
    if (!visible || !enabled || !mousePressed) return false;
    
    ofVec2f mousePos(args.x, args.y);
    
    // Vérifier si on commence à draguer
    if (!isDraggingValue) {
        float dragDistance = abs(mousePos.y - dragStartY);
        if (dragDistance > 2.0f) { // Seuil de drag plus petit pour les number box
            isDraggingValue = true;
        }
    }
    
    if (isDraggingValue) {
        // CORRECTION: Passer la position Y actuelle, pas le delta
        updateValueFromDrag(mousePos.y);
        lastMousePos = mousePos;
        return true;
    }
    
    return false;
}

bool PdNumberBox::onMouseReleased(ofMouseEventArgs& args) {
    bool wasPressed = mousePressed;
    bool wasDragging = isDraggingValue;
    
    mousePressed = false;
    isDraggingValue = false;
    
    if (wasPressed) {
        // Si on n'était pas en train de draguer, on peut implémenter
        // d'autres interactions (comme double-clic pour édition directe)
        if (!wasDragging) {
            // Ici on pourrait ajouter une fonctionnalité d'édition directe
            // avec un champ de texte
        }
        
        markForUpdate();
        return true;
    }
    
    return false;
}

bool PdNumberBox::onMouseMoved(ofMouseEventArgs& args) {
    if (!visible || !enabled) return false;
    
    ofVec2f mousePos(args.x, args.y);
    bool wasMouseOver = mouseOver;
    mouseOver = isPointInside(mousePos);
    
    if (wasMouseOver != mouseOver) {
        markForUpdate();
    }
    
    return mouseOver;
}

void PdNumberBox::setNumberValue(float value) {
    float oldValue = currentValue;
    setValue(value);
    
    // Envoyer la valeur à Pure Data si elle a changé
    if (abs(currentValue - oldValue) > 0.001f) {
        sendToPd(currentValue);
    }
}

float PdNumberBox::getNumberValue() const {
    return currentValue;
}

void PdNumberBox::setPrecision(int precision) {
    displayPrecision = max(0, min(precision, 10)); // Limiter entre 0 et 10
    markForUpdate();
}

void PdNumberBox::setDragSensitivity(float sensitivity) {
    dragSensitivity = max(0.01f, sensitivity); // Minimum pour éviter division par zéro
}

void PdNumberBox::setValueRange(float min, float max) {
    PdGuiObject::setValueRange(min, max);
}

string PdNumberBox::formatValue() const {
    if (displayPrecision == 0) {
        // Affichage entier
        return ofToString((int)round(currentValue));
    } else {
        // Affichage avec décimales
        return ofToString(currentValue, displayPrecision);
    }
}

void PdNumberBox::drawNumberText() {
    string valueStr = formatValue();
    
    // Couleur du texte selon l'état
    ofColor textColor = DEFAULT_FG_COLOR;
    if (isDraggingValue) {
        textColor = ofColor(0, 100, 200); // Bleu pendant le drag
    } else if (!enabled) {
        textColor = textColor * 0.5f;
    }
    
    ofSetColor(textColor);
    
    // Calculer la position pour centrer le texte
    float textWidth = valueStr.length() * 8; // Approximation pour bitmap font
    float textHeight = 8; // Hauteur approximative bitmap font
    
    // CORRECTION: Utiliser les coordonnées locales (0,0 = coin supérieur gauche de l'objet)
    float textX = (size.x - textWidth) * 0.5f;
    float textY = (size.y + textHeight) * 0.5f;
    
    // S'assurer que le texte reste dans les limites
    textX = max(2.0f, textX);
    textY = max(textHeight, textY);
    
    ofDrawBitmapString(valueStr, textX, textY);
}

void PdNumberBox::drawBackground() {
    ofColor bgColor = DEFAULT_BG_COLOR;
    
    if (isDraggingValue) {
        bgColor = ofColor(230, 240, 255); // Bleu clair pendant le drag
    } else if (mousePressed) {
        bgColor = PRESSED_COLOR;
    } else if (mouseOver) {
        bgColor = HOVER_COLOR;
    }
    
    if (!enabled) {
        bgColor = bgColor * 0.5f;
    }
    
    ofSetColor(bgColor);
    // CORRECTION: Utiliser les coordonnées locales
    ofDrawRectangle(0, 0, size.x, size.y);
}

float PdNumberBox::calculateDragDelta(float currentY) const {
    return (dragStartY - currentY) * dragSensitivity;
}

void PdNumberBox::updateValueFromDrag(float currentY) {
    // Calculer le delta depuis le début du drag
    float deltaY = dragStartY - currentY; // Positif = vers le haut = augmenter
    float valueDelta = deltaY * dragSensitivity;
    
    // Calculer la nouvelle valeur
    float newValue = dragStartValue + valueDelta;
    
    // Appliquer la précision pour éviter les valeurs trop précises pendant le drag
    if (displayPrecision == 0) {
        newValue = round(newValue);
    } else {
        float precision = pow(10.0f, displayPrecision);
        newValue = round(newValue * precision) / precision;
    }
    
    setNumberValue(newValue);
}
