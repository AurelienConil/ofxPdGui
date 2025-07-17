#include "Canvas.h"

PdCanvas::PdCanvas(ofVec2f position, ofVec2f size,
                   const string& label,
                   ofColor backgroundColor, ofColor textColor)
    : PdGuiObject(GuiType::UNKNOWN, position, size, "empty", "empty")
    , canvasLabel(label)
    , backgroundColor(backgroundColor)
    , textColor(textColor)  // Renommé de borderColor
    , labelFontSize(12)
{
    enabled = false;
    visible = true;
}

void PdCanvas::update() {
    // Rien à faire pour un canvas statique
}

void PdCanvas::draw() {
    ofPushStyle();
    
    // Dessiner le fond du canvas
    drawCanvasBackground();
    
    // Dessiner le label si présent
    if (!canvasLabel.empty()) {
        drawCanvasLabel();
    }
    
    // PAS de bordure dans Pure Data
    
    ofPopStyle();
}

// Toutes les méthodes de souris retournent false
bool PdCanvas::onMousePressed(ofMouseEventArgs& args) {
    return false;
}

bool PdCanvas::onMouseDragged(ofMouseEventArgs& args) {
    return false;
}

bool PdCanvas::onMouseReleased(ofMouseEventArgs& args) {
    return false;
}

bool PdCanvas::onMouseMoved(ofMouseEventArgs& args) {
    return false;
}

void PdCanvas::setColors(ofColor backgroundColor, ofColor textColor) {
    this->backgroundColor = backgroundColor;
    this->textColor = textColor;
    markForUpdate();
}

void PdCanvas::setLabel(const string& label) {
    this->canvasLabel = label;
    markForUpdate();
}

void PdCanvas::setLabelStyle(int fontSize) {
    this->labelFontSize = fontSize;
    markForUpdate();
}

void PdCanvas::drawCanvasBackground() {
    ofSetColor(backgroundColor);
    ofDrawRectangle(0, 0, size.x, size.y);
}

void PdCanvas::drawCanvasLabel() {
    if (canvasLabel.empty()) return;
    
    // Utiliser la couleur de texte pour le label
    ofSetColor(textColor);
    
    // Positionner le label en haut à gauche avec un petit margin
    float textX = 4;
    float textY = labelFontSize + 2;
    
    ofDrawBitmapString(canvasLabel, textX, textY);
}

// Surcharger les méthodes de base
void PdCanvas::drawBackground() {
    drawCanvasBackground();
}

void PdCanvas::drawBorder() {
    // Ne rien faire - pas de bordure dans Pure Data
}

void PdCanvas::drawLabel() {
    drawCanvasLabel();
}
