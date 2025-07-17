/**
 * @file Bang.h
 * @brief Implémentation du Bang Pure Data (objet 'bng')
 * 
 * RÔLE DANS L'ARCHITECTURE :
 * Le PdBang représente l'objet 'bng' de Pure Data - un bouton momentané qui 
 * émet un signal d'impulsion quand il est activé puis retourne automatiquement
 * à l'état inactif.
 * 
 * FONCTIONNALITÉS :
 * - Bouton momentané (pulse/impulsion)
 * - Activation par clic souris avec feedback visuel temporaire
 * - Retour automatique à l'état inactif après 300ms
 * - Cercle rouge visible pendant l'activation
 * - Intégration avec le système de callbacks pour ofxPd
 * 
 * COMPORTEMENT :
 * - Clic : Active le bang et envoie un signal vers Pure Data
 * - Animation : Cercle rouge visible pendant TRIGGER_DURATION
 * - Auto-reset : Retour automatique à l'état inactif
 * 
 * RENDU :
 * - Carré avec bordure noire (comme toggle)
 * - Cercle rouge centré quand activé
 * - Arrière-plan gris clair standard Pure Data
 * 
 * FORMAT PURE DATA :
 * #X obj x y bng size hold send receive label x_offset y_offset font_size
 *      bg_color fg_color label_color init_value default_value;
 * 
 * @author Aurélien Conil
 * @date 2025
 */

//
//  Bang.h
//  pd-gui
//
//  Created by Aurélien Conil on 15/07/2025.
//

/**
 * @class PdBang
 * @brief Implémentation de l'objet Bang Pure Data
 * 
 * Le bang est un contrôle d'impulsion de Pure Data. Contrairement au toggle,
 * il n'a pas d'état permanent - il s'active momentanément puis se désactive.
 * 
 * COMPORTEMENT :
 * - Clic : Active temporairement le bang (300ms)
 * - Envoi automatique d'un signal vers Pure Data via callback
 * - Feedback visuel avec cercle rouge
 * - Retour automatique à l'état inactif
 * 
 * TIMING :
 * - TRIGGER_DURATION = 300ms (constante Pure Data standard)
 * - Gestion automatique du timing dans update()
 * 
 * STYLE PURE DATA :
 * - Même style que toggle (carré + bordure noire)
 * - Cercle rouge centré pendant l'activation
 */
class PdBang : public PdGuiObject {
public:
    // === PALETTE DE COULEURS BANG ===
    /// Couleurs fixes pour respecter le style Pure Data
    static const ofColor BANG_BG_COLOR;      ///< Arrière-plan gris clair (#fcfcfc)
    static const ofColor BANG_BORDER_COLOR;  ///< Bordure noire
    static const ofColor BANG_CIRCLE_COLOR;  ///< Couleur du cercle d'activation (rouge)
    static const ofColor BANG_HOVER_COLOR;   ///< Couleur de survol
    
    // === CONSTRUCTION ===
    /// Constructeur du bang Pure Data
    /// @param position Position (x,y) dans l'espace de rendu
    /// @param size Taille (largeur, hauteur) - généralement carrée
    /// @param sendSymbol Symbole Pure Data pour l'envoi de valeurs
    /// @param receiveSymbol Symbole Pure Data pour la réception (optionnel)
    PdBang(ofVec2f position, ofVec2f size,
           const string& sendSymbol, const string& receiveSymbol = "");
    
    // === MÉTHODES VIRTUELLES HÉRITÉES ===
    /// Mise à jour logique - gestion du timing d'activation
    void update() override;
    
    /// Rendu du bang avec style Pure Data
    void draw() override;
    
    // === GESTION DES ÉVÉNEMENTS SOURIS ===
    /// Gestion du clic - active le bang
    bool onMousePressed(ofMouseEventArgs& args) override;
    
    /// Gestion du relâchement - finalise l'interaction
    bool onMouseReleased(ofMouseEventArgs& args) override;
    
    // === MÉTHODES SPÉCIFIQUES AU BANG ===
    /// Active manuellement le bang (programmation)
    void trigger();
    
    /// Vérifie si le bang est actuellement en état activé
    bool isTriggered() const { return triggered; }
    
private:
    // === ÉTAT DU BANG ===
    bool triggered;              ///< Bang actuellement activé
    float triggerTime;           ///< Temps depuis l'activation (pour le timeout)
    static const float TRIGGER_DURATION; ///< Durée d'activation = 300ms
    
    // === MÉTHODES DE DESSIN PRIVÉES ===
    /// Dessine l'état du bang (cercle rouge si activé)
    void drawBangState();
    
    /// Dessine la bordure du bang (identique au toggle)
    void drawBangBorder();
    
    /// Dessine le cercle d'activation au centre
    void drawCircle();
};
