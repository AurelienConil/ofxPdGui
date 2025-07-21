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

// Forward declaration  
struct GopProperties;

/**
 * @class PdSubpatch
 * @brief Implémentation de l'objet Subpatch Pure Data avec support GOP (Graph-on-Parent)
 * 
 * Cette classe gère exclusivement les subpatches Pure Data en mode GOP
 * (Graph-on-Parent, "Dessiner dans le patch parent"). Elle transforme
 * les coordonnées normalisées des objets enfants en coordonnées pixels
 * selon les propriétés GOP du subpatch.
 * 
 * FONCTIONNEMENT GOP :
 * - Seuls les subpatches avec GOP activé sont supportés
 * - Les coordonnées des objets enfants sont normalisées (ex: X:0-1, Y:-1 à 1)
 * - Transformation en coordonnées pixel selon la formule :
 *   pixelX = posParentX + ((objX - minX) / (maxX - minX)) * widthInPixels
 *   pixelY = posParentY + ((objY - minY) / (maxY - minY)) * heightInPixels
 * 
 * PROPRIÉTÉS GOP :
 * - minX, minY, maxX, maxY : Intervalles des coordonnées normalisées
 * - widthInPixels, heightInPixels : Taille du rectangle GOP en pixels
 * - Position dans le patch parent définie par les coordonnées du #X restore
 */
class PdSubpatch : public PdGuiObject {
public:
    // === CONSTRUCTION ET DESTRUCTION ===
    /// Constructeur principal - charge et initialise le subpatch GOP
    /// @param position Position du subpatch dans le patch parent
    /// @param sendSymbol Symbole Pure Data pour l'envoi
    /// @param receiveSymbol Symbole Pure Data pour la réception
    /// @param subpatchPath Chemin vers le fichier .pd du subpatch
    /// @param gopProps Propriétés GOP (intervalles et taille) du subpatch
    PdSubpatch(ofVec2f position,
               const std::string& sendSymbol, const std::string& receiveSymbol,
               const std::string& subpatchPath,
               const GopProperties& gopProps);
    
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
    
    /// Propriétés GOP du subpatch
    const GopProperties& getGopProperties() const { return gopProps; }
    
    /// Vérifie si le subpatch est en mode GOP
    bool isGopEnabled() const { return gopProps.isGop; }

private:
    // === PROPRIÉTÉS PRIVÉES ===
    std::vector<std::unique_ptr<PdGuiObject>> children;  ///< Collection des objets enfants
    std::string subpatchPath;                           ///< Chemin vers le fichier .pd
    GopProperties gopProps;                             ///< Propriétés GOP du subpatch
    
    // === MÉTHODES PRIVÉES ===
    /// Charge et parse le fichier subpatch
    bool loadSubpatch();
    
    /// Transforme les coordonnées des objets enfants selon le mapping GOP
    void transformChildrenCoordinates();
    
    /// Applique la transformation GOP à une position donnée
    ofVec2f transformGopCoordinates(const ofVec2f& localPos) const;
    
    /// Trouve l'objet enfant à une position donnée (pour délégation événements)
    PdGuiObject* findChildAt(ofVec2f position);
    
    /// Propage une propriété à tous les objets enfants
    void propagateToChildren(std::function<void(PdGuiObject*)> action);
    
    /// Configure les callbacks Pure Data pour les objets enfants
    void setupChildrenCallbacks();
};