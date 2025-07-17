/**
 * @file Slider.h  
 * @brief Implémentation des Sliders Pure Data (objets 'hsl' et 'vsl')
 * 
 * RÔLE DANS L'ARCHITECTURE :
 * Le PdSlider représente les objets 'hsl' (horizontal slider) et 'vsl' (vertical slider)
 * de Pure Data - des contrôles de valeur continue permettant de sélectionner une valeur
 * dans une plage donnée via interaction souris.
 * 
 * FONCTIONNALITÉS :
 * - Contrôle de valeur continue (plage min/max configurable)
 * - Support horizontal et vertical (déterminé par GuiType)
 * - Interaction par drag de la "poignée" (knob)
 * - Affichage optionnel de la valeur numérique
 * - Conversion automatique position ↔ valeur
 * - Intégration avec le système de callbacks pour ofxPd
 * 
 * COMPOSANTS VISUELS :
 * - Track (piste) : Zone de déplacement du knob
 * - Knob (poignée) : Élément mobile représentant la valeur
 * - Texte de valeur (optionnel) : Affichage numérique de la valeur
 * 
 * INTERACTIONS :
 * - Clic sur knob : Début du drag
 * - Drag : Modification continue de la valeur
 * - Relâchement : Fin de l'interaction
 * 
 * FORMAT PURE DATA :
 * Horizontal: #X obj x y hsl width height min max log init send receive label...
 * Vertical:   #X obj x y vsl width height min max log init send receive label...
 * 
 * @author Aurélien Conil
 * @date 2025
 */

//
//  Slider.h
//  pd-gui
//
//  Created by Aurélien Conil on 16/07/2025.
//

/**
 * @class PdSlider
 * @brief Implémentation des sliders horizontaux et verticaux Pure Data
 * 
 * Cette classe unifie les sliders horizontaux (hsl) et verticaux (vsl) de Pure Data.
 * Le comportement est adapté selon le type spécifié dans le constructeur.
 * 
 * ARCHITECTURE :
 * - Orientation déterminée par isHorizontal (calculé depuis GuiType)
 * - Track : Zone fixe de déplacement
 * - Knob : Élément mobile de taille configurable
 * - Conversion automatique entre position souris et valeur
 * 
 * PLAGE DE VALEURS :
 * - Par défaut : 0.0 à 127.0 (standard Pure Data)
 * - Configurable via setSliderRange()
 * - Mapping automatique vers la plage 0.0-1.0 pour currentValue
 * 
 * STYLE :
 * - Track : Rectangle avec bordure, style Pure Data
 * - Knob : Carré mobile, couleur différenciée
 * - Texte : Affichage optionnel de la valeur numérique
 */
class PdSlider : public PdGuiObject {
public:
    // === CONSTRUCTION ===
    /// Constructeur du slider Pure Data
    /// @param type HORIZONTAL_SLIDER ou VERTICAL_SLIDER
    /// @param position Position (x,y) dans l'espace de rendu
    /// @param size Taille (largeur, hauteur) - largeur=track pour hsl, hauteur=track pour vsl
    /// @param sendSymbol Symbole Pure Data pour l'envoi de valeurs
    /// @param receiveSymbol Symbole Pure Data pour la réception de valeurs
    /// @param min Valeur minimale de la plage (défaut: 0.0)
    /// @param max Valeur maximale de la plage (défaut: 127.0)
    /// @param initialValue Valeur initiale (défaut: 0.0)
    PdSlider(GuiType type, ofVec2f position, ofVec2f size,
             const string& sendSymbol, const string& receiveSymbol,
             float min = 0.0f, float max = 127.0f, float initialValue = 0.0f);
    
    // === MÉTHODES VIRTUELLES HÉRITÉES ===
    /// Mise à jour logique (actuellement vide - pas d'animation)
    virtual void update() override;
    
    /// Rendu du slider complet (track + knob + texte)
    virtual void draw() override;
    
    // === GESTION DES ÉVÉNEMENTS SOURIS ===
    /// Gestion du clic - détection du knob et début du drag
    virtual bool onMousePressed(ofMouseEventArgs& args) override;
    
    /// Gestion du drag - modification continue de la valeur
    virtual bool onMouseDragged(ofMouseEventArgs& args) override;
    
    /// Gestion du relâchement - fin du drag
    virtual bool onMouseReleased(ofMouseEventArgs& args) override;
    
    // === MÉTHODES SPÉCIFIQUES AU SLIDER ===
    /// Définit la valeur du slider dans sa plage native (min/max)
    void setSliderValue(float value);
    
    /// Récupère la valeur du slider dans sa plage native (min/max)
    float getSliderValue() const;
    
    /// Configure la plage de valeurs du slider
    void setSliderRange(float min, float max);
    
    /// Configure le style du slider
    /// @param knobSize Taille du knob en pixels (défaut: 8.0)
    /// @param showValue Afficher la valeur numérique (défaut: true)
    void setSliderStyle(float knobSize = 8.0f, bool showValue = true);
    
protected:
    // === PROPRIÉTÉS DE STYLE ===
    float knobSize;              ///< Taille du knob en pixels
    bool showValue;              ///< Afficher la valeur numérique
    bool isHorizontal;           ///< true=horizontal, false=vertical
    
    // === CALCULS GÉOMÉTRIQUES ===
    /// Calcule la position actuelle du knob selon la valeur
    ofVec2f getKnobPosition() const;
    
    /// Récupère le rectangle de collision du knob
    ofRectangle getKnobBounds() const;
    
    /// Récupère le rectangle de la piste (track)
    ofRectangle getTrackBounds() const;
    
    // === CONVERSIONS POSITION ↔ VALEUR ===
    /// Convertit une position souris en valeur slider
    float positionToValue(ofVec2f mousePos) const;
    
    /// Convertit la valeur actuelle en position de knob
    ofVec2f valueToPosition() const;
    
    // === MÉTHODES DE DESSIN SPÉCIALISÉES ===
    /// Dessine la piste (track) du slider
    void drawTrack();
    
    /// Dessine le knob mobile
    void drawKnob();
    
    /// Dessine la valeur numérique (si showValue=true)
    void drawValueText();
    
private:
    // === VARIABLES DE DRAG ===
    ofVec2f dragOffset;          ///< Offset entre clic souris et centre du knob
    bool isDraggingKnob;         ///< Drag du knob en cours
};
