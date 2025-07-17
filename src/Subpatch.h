/**
 * @file Subpatch.h
 * @brief Implémentation du Subpatch Pure Data (méthode "flat")
 * 
 * RÔLE DANS L'ARCHITECTURE :
 * La classe PdSubpatch permet l'intégration d'externals et de sous-patches 
 * Pure Data dans la fenêtre graphique en utilisant la méthode "flat".
 * 
 * CONCEPT DE LA MÉTHODE "FLAT" :
 * Tous les objets du sous-patch (sliders, bangs, etc.) sont instanciés 
 * comme s'ils étaient dans le patch principal. Leurs coordonnées sont 
 * absolues et du point de vue d'openFrameworks, on ne distingue pas 
 * vraiment qu'ils sont dans un "sous-patch".
 * 
 * FONCTIONNALITÉS :
 * - Chargement et parsing des fichiers de sous-patches (.pd)
 * - Instanciation des objets GUI contenus dans le sous-patch
 * - Transformation des coordonnées relatives en coordonnées absolues
 * - Gestion de la collection des objets enfants
 * - Délégation des événements souris aux objets enfants appropriés
 * - Propagation des mises à jour et des états (visible, enabled)
 * - Support de la récursivité (subpatches dans des subpatches)
 * 
 * INTÉGRATION :
 * - Hérite de PdGuiObject comme les autres classes GUI
 * - Compatible avec le système de rendu et FBO existant
 * - Transparent pour le système d'événements
 * 
 * FORMAT PURE DATA :
 * Les subpatches peuvent être définis via :
 * - #N canvas x y width height name : Début de subpatch
 * - #X restore x y pd name : Fin de subpatch (référence dans patch parent)
 * 
 * @author Aurélien Conil
 * @date 2025
 */

#pragma once

#include "ofMain.h"
#include "PdGuiObject.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @class PdSubpatch
 * @brief Implémentation de l'objet Subpatch Pure Data avec méthode "flat"
 * 
 * Cette classe permet d'intégrer des sous-patches Pure Data en instanciant
 * tous leurs objets GUI comme s'ils étaient dans le patch principal.
 * 
 * ARCHITECTURE "FLAT" :
 * - Les objets enfants ont des coordonnées absolues dans l'espace du patch principal
 * - Pas de rendu hiérarchique - tous les objets sont au même niveau
 * - Délégation transparente des événements aux objets appropriés
 * - Support de la récursivité pour les subpatches imbriqués
 * 
 * GESTION DES OBJETS ENFANTS :
 * - Chargement automatique du fichier .pd du subpatch
 * - Transformation des coordonnées relatives en absolues
 * - Gestion du cycle de vie des objets enfants
 * - Propagation des états et propriétés
 */
class PdSubpatch : public PdGuiObject {
public:
    // === CONSTRUCTION ET DESTRUCTION ===
    /// Constructeur principal - charge et initialise le subpatch
    /// @param position Position du subpatch dans le patch parent
    /// @param size Taille du subpatch (peut être utilisée pour clipping)
    /// @param sendSymbol Symbole Pure Data pour l'envoi
    /// @param receiveSymbol Symbole Pure Data pour la réception
    /// @param subpatchPath Chemin vers le fichier .pd du subpatch
    /// @param offsetX Décalage X pour transformer les coordonnées enfants
    /// @param offsetY Décalage Y pour transformer les coordonnées enfants
    PdSubpatch(ofVec2f position, ofVec2f size,
               const std::string& sendSymbol, const std::string& receiveSymbol,
               const std::string& subpatchPath,
               float offsetX = 0.0f, float offsetY = 0.0f);
    
    /// Destructeur - nettoie les objets enfants
    virtual ~PdSubpatch() = default;
    
    // === MÉTHODES VIRTUELLES HÉRITÉES ===
    /// Mise à jour de tous les objets enfants
    virtual void update() override;
    
    /// Rendu de tous les objets enfants (méthode flat)
    virtual void draw() override;
    
    // === GESTION DES ÉVÉNEMENTS SOURIS ===
    /// Délègue les événements de clic aux objets enfants appropriés
    virtual bool onMousePressed(ofMouseEventArgs& args) override;
    
    /// Délègue les événements de drag aux objets enfants appropriés
    virtual bool onMouseDragged(ofMouseEventArgs& args) override;
    
    /// Délègue les événements de relâchement aux objets enfants appropriés
    virtual bool onMouseReleased(ofMouseEventArgs& args) override;
    
    /// Délègue les événements de mouvement aux objets enfants appropriés
    virtual bool onMouseMoved(ofMouseEventArgs& args) override;
    
    // === GESTION DES VALEURS ===
    /// Définit la valeur du subpatch (peut être propagée aux enfants)
    virtual void setValue(float value) override;
    
    /// Propagation des états aux objets enfants
    virtual void setVisible(bool visible);
    virtual void setEnabled(bool enabled);
    
    // === GESTION DES OBJETS ENFANTS ===
    /// Accès aux objets enfants (lecture seule)
    const std::vector<std::unique_ptr<PdGuiObject>>& getChildren() const { return children; }
    
    /// Nombre d'objets enfants
    size_t getChildCount() const { return children.size(); }
    
    /// Recharge le subpatch depuis le fichier
    bool reload();
    
    /// Ajoute un objet enfant manuellement (pour subpatches dynamiques)
    void addChild(std::unique_ptr<PdGuiObject> child);
    
    /// Supprime tous les objets enfants
    void clearChildren();
    
    // === PROPRIÉTÉS SPÉCIFIQUES ===
    /// Chemin du fichier subpatch
    const std::string& getSubpatchPath() const { return subpatchPath; }
    
    /// Décalages pour la transformation des coordonnées
    ofVec2f getOffset() const { return ofVec2f(offsetX, offsetY); }
    void setOffset(float x, float y) { offsetX = x; offsetY = y; transformChildrenCoordinates(); }
    
private:
    // === PROPRIÉTÉS PRIVÉES ===
    std::vector<std::unique_ptr<PdGuiObject>> children;  ///< Collection des objets enfants
    std::string subpatchPath;                           ///< Chemin vers le fichier .pd
    float offsetX, offsetY;                             ///< Décalages pour transformation coords
    
    // === MÉTHODES PRIVÉES ===
    /// Charge et parse le fichier subpatch
    bool loadSubpatch();
    
    /// Transforme les coordonnées des objets enfants en coordonnées absolues
    void transformChildrenCoordinates();
    
    /// Trouve l'objet enfant à une position donnée (pour délégation événements)
    PdGuiObject* findChildAt(ofVec2f position);
    
    /// Propage une propriété à tous les objets enfants
    void propagateToChildren(std::function<void(PdGuiObject*)> action);
    
    /// Configure les callbacks Pure Data pour les objets enfants
    void setupChildrenCallbacks();
};