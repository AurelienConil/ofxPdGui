/**
 * @file Canvas.h
 * @brief Implémentation du Canvas Pure Data (objet 'cnv')
 * 
 * RÔLE DANS L'ARCHITECTURE :
 * Le PdCanvas représente l'objet 'cnv' de Pure Data - un élément d'affichage statique
 * utilisé principalement pour les labels, les arrière-plans et l'organisation visuelle
 * de l'interface.
 * 
 * FONCTIONNALITÉS :
 * - Affichage de texte/label configurable
 * - Arrière-plan coloré personnalisable
 * - Élément non-interactif (ne capture pas les événements souris)
 * - Support des couleurs Pure Data (background et texte)
 * - Taille de police configurable
 * 
 * COMPORTEMENT :
 * - Purement décoratif/informatif
 * - Ne réagit PAS aux événements souris (les laisse passer)
 * - Utilisé pour l'organisation et la documentation des patches
 * - Peut servir d'arrière-plan pour grouper d'autres contrôles
 * 
 * RENDU :
 * - Rectangle coloré (couleur d'arrière-plan configurable)
 * - Texte du label avec couleur configurable
 * - Pas de bordure (contrairement aux autres objets GUI)
 * - Style minimaliste cohérent avec Pure Data
 * 
 * FORMAT PURE DATA :
 * #X obj x y cnv size width height send receive label x_offset y_offset font_size
 *      bg_color fg_color label_color...
 * 
 * @author Aurélien Conil
 * @date 2025
 */

#pragma once

/**
 * @class PdCanvas
 * @brief Implémentation de l'objet Canvas Pure Data
 * 
 * Le canvas est un élément d'affichage passif de Pure Data utilisé pour
 * l'organisation visuelle et l'affichage d'informations.
 * 
 * CARACTÉRISTIQUES :
 * - Non-interactif : Ne capture aucun événement souris
 * - Configurable : Couleurs et texte personnalisables
 * - Organisationnel : Sert à structurer visuellement les patches
 * 
 * DIFFÉRENCES AVEC LES AUTRES OBJETS GUI :
 * - Pas de bordure par défaut
 * - Événements souris retournent false (ne les consomment pas)
 * - Focus sur l'affichage plutôt que l'interaction
 * 
 * UTILISATION TYPIQUE :
 * - Labels de sections dans un patch
 * - Arrière-plans colorés pour grouper des contrôles
 * - Informations textuelles statiques
 */
class PdCanvas : public PdGuiObject {
public:
    // === CONSTRUCTION ===
    /// Constructeur du canvas Pure Data
    /// @param position Position (x,y) dans l'espace de rendu
    /// @param size Taille (largeur, hauteur) du canvas
    /// @param label Texte à afficher (défaut: "")
    /// @param backgroundColor Couleur d'arrière-plan (défaut: gris clair)
    /// @param textColor Couleur du texte (défaut: noir)
    PdCanvas(ofVec2f position, ofVec2f size,
             const string& label = "",
             ofColor backgroundColor = ofColor(224, 224, 224),
             ofColor textColor = ofColor(0, 0, 0));
    
    // === MÉTHODES VIRTUELLES HÉRITÉES ===
    /// Mise à jour logique (vide - élément statique)
    virtual void update() override;
    
    /// Rendu du canvas avec arrière-plan et label
    virtual void draw() override;
    
    // === ÉVÉNEMENTS SOURIS (NON-INTERACTIF) ===
    /// Retourne toujours false - ne capture pas les événements
    virtual bool onMousePressed(ofMouseEventArgs& args) override;
    virtual bool onMouseDragged(ofMouseEventArgs& args) override;
    virtual bool onMouseReleased(ofMouseEventArgs& args) override;
    virtual bool onMouseMoved(ofMouseEventArgs& args) override;
    
    // === CONFIGURATION SPÉCIFIQUE AU CANVAS ===
    /// Configure les couleurs du canvas
    void setColors(ofColor backgroundColor, ofColor textColor);
    
    /// Définit le texte du label
    void setLabel(const string& label);
    
    /// Configure le style du label
    /// @param fontSize Taille de la police (défaut: 12)
    void setLabelStyle(int fontSize = 12);
    
protected:
    // === PROPRIÉTÉS SPÉCIFIQUES ===
    string canvasLabel;          ///< Texte du label à afficher
    ofColor backgroundColor;     ///< Couleur d'arrière-plan du canvas
    ofColor textColor;           ///< Couleur du texte (renommé de borderColor)
    int labelFontSize;           ///< Taille de la police du label
    
    // === MÉTHODES DE DESSIN SPÉCIALISÉES ===
    /// Dessine l'arrière-plan coloré du canvas
    void drawCanvasBackground();
    
    /// Dessine le label avec la couleur et taille configurées
    void drawCanvasLabel();
    
    // === SURCHARGE DES MÉTHODES DE DESSIN DE BASE ===
    /// Utilise drawCanvasBackground() au lieu du dessin standard
    virtual void drawBackground() override;
    
    /// Ne dessine aucune bordure (canvas sans bordure)
    virtual void drawBorder() override;
    
    /// Utilise drawCanvasLabel() au lieu du dessin standard
    virtual void drawLabel() override;
};
