/**
 * @file Toggle.h
 * @brief Implémentation du Toggle/Checkbox Pure Data (objet 'tgl')
 * 
 * RÔLE DANS L'ARCHITECTURE :
 * Le PdToggle représente l'objet 'tgl' de Pure Data - un bouton à deux états 
 * (ON/OFF) qui peut être activé/désactivé par clic souris.
 * 
 * FONCTIONNALITÉS :
 * - État binaire : ON (1.0) ou OFF (0.0)
 * - Activation par clic souris
 * - Rendu fidèle au style Pure Data
 * - Intégration avec le système de callbacks pour ofxPd
 * 
 * RENDU :
 * - Carré avec bordure noire
 * - Arrière-plan gris clair en mode OFF
 * - Croix noire en mode ON
 * - Feedback visuel lors du survol
 * 
 * FORMAT PURE DATA :
 * #X obj x y tgl size nonzero send receive label x_offset y_offset font_size 
 *      bg_color fg_color label_color init_value default_value;
 * 
 * @author Aurélien Conil  
 * @date 2025
 */

//
//  toogle.h
//  pd-gui
//
//  Created by Aurélien Conil on 15/07/2025.
//

/**
 * @class PdToggle
 * @brief Implémentation de l'objet Toggle Pure Data
 * 
 * Le toggle est un contrôle binaire fondamental de Pure Data.
 * Il alterne entre deux états (ON/OFF) via des clics souris.
 * 
 * COMPORTEMENT :
 * - Clic : bascule entre ON (1.0) et OFF (0.0)
 * - Envoi automatique de la nouvelle valeur via callback
 * - Rendu avec croix visible en état ON
 * 
 * STYLE PURE DATA :
 * - Carré avec bordure noire fixe
 * - Arrière-plan gris (#fcfcfc) 
 * - Croix noire en mode activé
 * - Couleur de survol pour le feedback
 */
class PdToggle : public PdGuiObject {
public:
    // === CONSTRUCTION ===
    /// Constructeur du toggle Pure Data
    /// @param position Position (x,y) dans l'espace de rendu
    /// @param size Taille (largeur, hauteur) - généralement carrée
    /// @param sendSymbol Symbole Pure Data pour l'envoi de valeurs
    /// @param receiveSymbol Symbole Pure Data pour la réception de valeurs
    PdToggle(ofVec2f position, ofVec2f size,
             const string& sendSymbol, const string& receiveSymbol);
    
    // === MÉTHODES VIRTUELLES HÉRITÉES ===
    /// Mise à jour logique (actuellement vide - pas d'animation)
    void update() override;
    
    /// Rendu du toggle avec style Pure Data
    void draw() override;
    
    // === GESTION DES ÉVÉNEMENTS SOURIS ===
    /// Gestion du clic - bascule l'état du toggle
    bool onMousePressed(ofMouseEventArgs& args) override;
    
    /// Gestion du relâchement - finalise l'interaction
    bool onMouseReleased(ofMouseEventArgs& args) override;
    
    // === MÉTHODES SPÉCIFIQUES AU TOGGLE ===
    /// Bascule l'état ON/OFF du toggle
    void toggle();
    
    /// Vérifie si le toggle est en état ON
    bool isOn() const { return currentValue > 0.5f; }
    
    /// Définit l'état ON/OFF directement
    void setOn(bool on) { setValue(on ? 1.0f : 0.0f); }
    
private:
    // === PALETTE DE COULEURS TOGGLE ===
    /// Couleurs fixes pour respecter le style Pure Data
    static const ofColor TOGGLE_BG_COLOR;      ///< Arrière-plan gris clair (#fcfcfc)
    static const ofColor TOGGLE_BORDER_COLOR;  ///< Bordure noire
    static const ofColor TOGGLE_ON_COLOR;      ///< Couleur de la croix (noir)
    static const ofColor TOGGLE_OFF_COLOR;     ///< Couleur en mode OFF (transparent)
    static const ofColor TOGGLE_HOVER_COLOR;   ///< Couleur de survol
    
    // === MÉTHODES DE DESSIN PRIVÉES ===
    /// Dessine l'état du toggle (croix si ON, vide si OFF)
    void drawToggleState();
    
    /// Dessine la bordure du toggle
    void drawToggleBorder();
};
