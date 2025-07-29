/**
 * @file Subpatch.cpp
 * @brief Implémentation de la classe PdSubpatch pour le support des sous-patches
 * 
 * Ce fichier contient l'implémentation complète de la classe PdSubpatch qui
 * permet l'intégration de sous-patches Pure Data en utilisant ofVec2f PdSubpatch::transformGopCoordinates(const ofVec2f& canvasCoords) {
    // Les coordonnées doivent être RELATIVES au subpatch, pas absolues
    // Dans l'architecture actuelle, ofApp::drawGuiObjects() fait déjà un ofTranslate()
    // pour positionner le subpatch, donc les enfants doivent être en coordonnées locales
    
    // Transformation GOP : du canvas vers l'espace GOP local
    float localX = (canvasCoords.x - gopProps.minX) / (gopProps.maxX - gopProps.minX) * gopProps.widthInPixels;
    float localY = (canvasCoords.y - gopProps.minY) / (gopProps.maxY - gopProps.minY) * gopProps.heightInPixels;
    
    ofLogNotice("PdSubpatch") << "GOP transform: canvas(" << canvasCoords.x << "," << canvasCoords.y 
                              << ") -> local(" << localX << "," << localY << ")";
    
    return ofVec2f(localX, localY);
}at".
 * 
 * ARCHITECTURE D'IMPLÉMENTATION :
 * 1. Chargement du fichier .pd du subpatch
 * 2. Parsing et instanciation des objets enfants
 * 3. Transformation des coordonnées relatives en absolues
 * 4. Délégation des événements et du rendu aux objets enfants
 * 
 * MÉTHODE "FLAT" :
 * - Tous les objets enfants sont traités comme s'ils étaient dans le patch principal
 * - Leurs coordonnées sont transformées en coordonnées absolues
 * - Le rendu et les événements sont délégués directement aux enfants
 * - Pas de hiérarchie visuelle - intégration transparente
 * 
 * GESTION DES CALLBACKS :
 * - Les objets enfants utilisent les mêmes callbacks que les objets principaux
 * - Communication Pure Data transparente
 * - Support des send/receive symbols individuels
 */

#include "Subpatch.h"
#include "PatchParser.h"

PdSubpatch::PdSubpatch(ofVec2f position,
                       const std::string& sendSymbol, const std::string& receiveSymbol,
                       const std::string& subpatchPath,
                       const GopProperties& gopProps,
                       const std::vector<std::string>& inlineContent,
                       const ofVec2f& canvasSize)
    : PdGuiObject(GuiType::SUBPATCH, position, ofVec2f(gopProps.widthInPixels, gopProps.heightInPixels), sendSymbol, receiveSymbol)
    , subpatchPath(subpatchPath)
    , gopProps(gopProps)
    , inlineContent(inlineContent)
    , canvasSize(canvasSize)
{
    // Vérifier que le subpatch est en mode GOP
    if (!gopProps.isGop) {
        ofLogError("PdSubpatch") << "Subpatch " << subpatchPath << " is not GOP-enabled. Only GOP subpatches are supported.";
        return;
    }
    
    // Charger le subpatch
    loadSubpatch();
}

void PdSubpatch::update() {
    // Méthode "flat" : déléguer la mise à jour à tous les objets enfants
    for (auto& child : children) {
        if (child && child->isVisible() && child->isEnabled()) {
            child->update();
        }
    }
}

void PdSubpatch::draw() {
    // Dessiner le contour de la zone GOP du subpatch
    ofPushStyle();
    ofSetColor(100, 100, 255, 128); // Bleu semi-transparent
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(0, 0, getSize().x, getSize().y);
    
    // Optionnel : ajouter un label
    ofSetColor(80, 80, 200);
    ofDrawBitmapString("GOP", 5, 15);
    ofPopStyle();
    
    // Méthode "flat" avec transformation correcte pour chaque enfant
    // Reproduire la logique d'ofApp::drawGuiObjects() pour les enfants
    for (int i = 0; i < children.size(); i++) {
        auto& child = children[i];
        if (child && child->isVisible()) {
            // Même logique que ofApp::drawGuiObjects() : ofTranslate + draw
            ofPushMatrix();
            ofTranslate(child->getPosition().x, child->getPosition().y);
            child->draw();
            ofPopMatrix();
        }
    }
}

bool PdSubpatch::onMousePressed(ofMouseEventArgs& args) {
    // Déléguer l'événement à l'objet enfant approprié
    PdGuiObject* targetChild = findChildAt(ofVec2f(args.x, args.y));
    
    if (targetChild && targetChild->isEnabled()) {
        return targetChild->onMousePressed(args);
    }
    
    return false;
}

bool PdSubpatch::onMouseDragged(ofMouseEventArgs& args) {
    // Déléguer l'événement à l'objet enfant approprié
    PdGuiObject* targetChild = findChildAt(ofVec2f(args.x, args.y));
    
    if (targetChild && targetChild->isEnabled()) {
        return targetChild->onMouseDragged(args);
    }
    
    return false;
}

bool PdSubpatch::onMouseReleased(ofMouseEventArgs& args) {
    // Déléguer l'événement à l'objet enfant approprié
    PdGuiObject* targetChild = findChildAt(ofVec2f(args.x, args.y));
    
    if (targetChild && targetChild->isEnabled()) {
        return targetChild->onMouseReleased(args);
    }
    
    return false;
}

bool PdSubpatch::onMouseMoved(ofMouseEventArgs& args) {
    // Déléguer l'événement à tous les objets enfants pour la gestion du hover
    bool handled = false;
    
    for (auto& child : children) {
        if (child && child->isEnabled()) {
            if (child->onMouseMoved(args)) {
                handled = true;
            }
        }
    }
    
    return handled;
}

void PdSubpatch::setValue(float value) {
    // Mettre à jour la valeur de base (appeler la méthode parent)
    PdGuiObject::setValue(value);
    
    // Optionnel : propager la valeur aux objets enfants
    // (dépend du comportement souhaité)
}

void PdSubpatch::setVisible(bool visible) {
    // Mettre à jour la visibilité de base (appeler la méthode parent)
    PdGuiObject::setVisible(visible);
    
    // Propager aux objets enfants
    propagateToChildren([visible](PdGuiObject* child) {
        child->setVisible(visible);
    });
}

void PdSubpatch::setEnabled(bool enabled) {
    // Mettre à jour l'état enabled de base (appeler la méthode parent)
    PdGuiObject::setEnabled(enabled);
    
    // Propager aux objets enfants
    propagateToChildren([enabled](PdGuiObject* child) {
        child->setEnabled(enabled);
    });
}

bool PdSubpatch::reload() {
    // Nettoyer les objets enfants existants
    clearChildren();
    
    // Recharger le subpatch
    return loadSubpatch();
}

void PdSubpatch::addChild(std::unique_ptr<PdGuiObject> child) {
    if (child) {
        // Les coordonnées des objets enfants doivent être relatives à l'origine de la zone GOP
        ofVec2f childPos = child->getPosition();
        
        // Soustraire l'origine de la zone graphique GOP pour obtenir les coordonnées relatives
        // Dans #X coords 0 -1 1 1 200 60 1 100 100, les derniers 100 100 sont l'origine
        ofVec2f relativePos = childPos - ofVec2f(100, 100); // TODO: utiliser gopProps.originX/Y
        
        ofLogNotice("PdSubpatch") << "Child canvas pos (" << childPos.x << ", " << childPos.y 
                                 << ") - GOP origin (100, 100) = relative (" 
                                 << relativePos.x << ", " << relativePos.y << ")";
        
        child->setPosition(relativePos);
        
        // Configurer les callbacks pour l'objet enfant
        child->onSendToPd = this->onSendToPd;
        child->onSendToPdString = this->onSendToPdString;
        
        children.push_back(std::move(child));
    }
}

void PdSubpatch::clearChildren() {
    children.clear();
}

bool PdSubpatch::loadSubpatch() {
    ofLogNotice("PdSubpatch") << "loadSubpatch() called with " << inlineContent.size() << " inline content lines";
    try {
        // Vérifier si on a du contenu inline
        if (!inlineContent.empty()) {
            ofLogNotice("PdSubpatch") << "Processing inline content...";
            // Traiter le contenu inline
            PdPatchParser parser;
            for (const auto& line : inlineContent) {
                ofLogNotice("PdSubpatch") << "Processing inline line: " << line;
                auto obj = parser.parseLine(line);
                if (obj) {
                    ofLogNotice("PdSubpatch") << "Created object, calling addChild()";
                    addChild(std::move(obj));
                } else {
                    ofLogNotice("PdSubpatch") << "No object created from line: " << line;
                }
            }
            
            ofLogNotice("PdSubpatch") << "Successfully loaded inline subpatch content with " 
                                      << children.size() << " objects";
            return true;
        }
        
        // Sinon, essayer de charger depuis un fichier externe
        if (!subpatchPath.empty()) {
            PdPatchParser parser;
            auto subpatchObjects = parser.parseFile(subpatchPath);
            
            // Ajouter les objets (la transformation des coordonnées se fait dans addChild)
            for (auto& obj : subpatchObjects) {
                if (obj) {
                    addChild(std::move(obj));
                }
            }
            
            ofLogNotice("PdSubpatch") << "Successfully loaded external subpatch: " << subpatchPath 
                                      << " with " << children.size() << " objects";
            return true;
        }
        
        ofLogWarning("PdSubpatch") << "No content available for subpatch (neither inline nor external file)";
        return false;
        
    } catch (const std::exception& e) {
        ofLogError("PdSubpatch") << "Failed to load subpatch " << subpatchPath 
                                 << ": " << e.what();
        return false;
    }
}

void PdSubpatch::transformChildrenCoordinates() {
    // Transformer les coordonnées de tous les objets enfants selon le mapping GOP
    for (auto& child : children) {
        if (child) {
            ofVec2f childPos = child->getPosition();
            ofVec2f transformedPos = transformGopCoordinates(childPos);
            child->setPosition(transformedPos);
        }
    }
    
    ofLogNotice("PdSubpatch") << "Transformed coordinates for " << children.size() 
                              << " children using GOP mapping (minX:" << gopProps.minX 
                              << ", minY:" << gopProps.minY << ", maxX:" << gopProps.maxX 
                              << ", maxY:" << gopProps.maxY << ")";
}

ofVec2f PdSubpatch::transformGopCoordinates(const ofVec2f& canvasPos) const {
    // Approche simple selon votre intuition :
    // Les coordonnées du canvas sont directement placées dans la zone GOP
    // Zone GOP : position + coordonnées relatives
    
    float pixelX = position.x + canvasPos.x;
    float pixelY = position.y + canvasPos.y;
    
    ofLogNotice("PdSubpatch") << "Simple transform: canvas(" << canvasPos.x << "," << canvasPos.y 
                              << ") + GOP base(" << position.x << "," << position.y 
                              << ") = final(" << pixelX << "," << pixelY << ")";
    
    return ofVec2f(pixelX, pixelY);
}

// Note: La fonction canvasToGopCoordinates() a été supprimée car elle était incorrecte.
// En Pure Data GOP, les coordonnées des objets sont directement dans l'espace GOP
// défini par les coords, pas dans l'espace du canvas parent.

PdGuiObject* PdSubpatch::findChildAt(ofVec2f position) {
    // Trouver l'objet enfant à la position donnée
    for (auto& child : children) {
        if (child && child->isPointInside(position)) {
            return child.get();
        }
    }
    
    return nullptr;
}

void PdSubpatch::propagateToChildren(std::function<void(PdGuiObject*)> action) {
    for (auto& child : children) {
        if (child) {
            action(child.get());
        }
    }
}

void PdSubpatch::setupChildrenCallbacks() {
    // Configurer les callbacks Pure Data pour tous les objets enfants
    propagateToChildren([this](PdGuiObject* child) {
        child->onSendToPd = this->onSendToPd;
        child->onSendToPdString = this->onSendToPdString;
    });
}
