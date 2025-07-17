/**
 * @file Subpatch.cpp
 * @brief Implémentation de la classe PdSubpatch pour le support des sous-patches
 * 
 * Ce fichier contient l'implémentation complète de la classe PdSubpatch qui
 * permet l'intégration de sous-patches Pure Data en utilisant la méthode "flat".
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

PdSubpatch::PdSubpatch(ofVec2f position, ofVec2f size,
                       const std::string& sendSymbol, const std::string& receiveSymbol,
                       const std::string& subpatchPath,
                       float offsetX, float offsetY)
    : PdGuiObject(GuiType::SUBPATCH, position, size, sendSymbol, receiveSymbol)
    , subpatchPath(subpatchPath)
    , offsetX(offsetX)
    , offsetY(offsetY)
{
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
    // Méthode "flat" : les objets enfants sont rendus directement
    // Pas de transformation ni de clipping - ils ont déjà les bonnes coordonnées absolues
    for (auto& child : children) {
        if (child && child->isVisible()) {
            child->draw();
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
    // Mettre à jour la valeur de base
    PdGuiObject::setValue(value);
    
    // Optionnel : propager la valeur aux objets enfants
    // (dépend du comportement souhaité)
}

void PdSubpatch::setVisible(bool visible) {
    // Mettre à jour la visibilité de base
    PdGuiObject::setVisible(visible);
    
    // Propager aux objets enfants
    propagateToChildren([visible](PdGuiObject* child) {
        child->setVisible(visible);
    });
}

void PdSubpatch::setEnabled(bool enabled) {
    // Mettre à jour l'état enabled de base
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
        // Transformer les coordonnées de l'objet enfant en coordonnées absolues
        ofVec2f childPos = child->getPosition();
        childPos.x += offsetX;
        childPos.y += offsetY;
        child->setPosition(childPos);
        
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
    try {
        // Utiliser le parser existant pour charger le subpatch
        PdPatchParser parser;
        auto subpatchObjects = parser.parseFile(subpatchPath);
        
        // Ajouter les objets (la transformation des coordonnées se fait dans addChild)
        for (auto& obj : subpatchObjects) {
            if (obj) {
                addChild(std::move(obj));
            }
        }
        
        ofLogNotice("PdSubpatch") << "Successfully loaded subpatch: " << subpatchPath 
                                  << " with " << children.size() << " objects";
        return true;
        
    } catch (const std::exception& e) {
        ofLogError("PdSubpatch") << "Failed to load subpatch " << subpatchPath 
                                 << ": " << e.what();
        return false;
    }
}

void PdSubpatch::transformChildrenCoordinates() {
    // Transformer les coordonnées de tous les objets enfants en coordonnées absolues
    for (auto& child : children) {
        if (child) {
            ofVec2f childPos = child->getPosition();
            childPos.x += offsetX;
            childPos.y += offsetY;
            child->setPosition(childPos);
        }
    }
    
    ofLogNotice("PdSubpatch") << "Transformed coordinates for " << children.size() 
                              << " children with offset (" << offsetX << ", " << offsetY << ")";
}

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