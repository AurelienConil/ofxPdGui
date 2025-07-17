
/**
 * @file PdGuiObject.h
 * @brief Classe de base pour tous les objets GUI Pure Data
 * 
 * RÔLE DANS L'ARCHITECTURE :
 * PdGuiObject est la classe de base abstraite qui définit l'interface commune
 * pour tous les objets GUI Pure Data. Elle constitue le cœur du système de rendu
 * et fournit l'infrastructure nécessaire pour :
 * 
 * RESPONSABILITÉS PRINCIPALES :
 * - Interface standardisée pour tous les objets GUI (draw, update, événements)
 * - Système de gestion des événements souris unifié
 * - Architecture de callbacks vers Pure Data (via ofxPd)
 * - Système d'optimisation FBO avec zones de mise à jour
 * - Gestion des propriétés communes (position, taille, visibilité, etc.)
 * 
 * SYSTÈME D'ÉVÉNEMENTS :
 * - onMousePressed/Dragged/Released/Moved : Gestion des interactions souris
 * - Hit testing automatique avec isPointInside()
 * - Propagation des événements depuis ofApp vers les objets concernés
 * 
 * SYSTÈME FBO ET OPTIMISATION :
 * - markForUpdate() : Marque les zones qui ont besoin d'être redessinées
 * - updateRegion : Suivi précis des zones modifiées
 * - drawToFbo() : Rendu optimisé dans un Frame Buffer Object
 * 
 * ARCHITECTURE DE CALLBACKS :
 * - onSendToPd : Callback pour envoyer des valeurs numériques vers Pure Data
 * - onSendToPdString : Callback pour envoyer des messages texte vers Pure Data
 * - Prêt pour l'intégration ofxPd transparente
 * 
 * CLASSES DÉRIVÉES IMPLÉMENTÉES :
 * - PdToggle : Checkbox/bouton toggle
 * - PdBang : Bouton momentané  
 * - PdSlider : Sliders horizontaux et verticaux
 * - PdNumberBox : Boîte de saisie numérique
 * - PdCanvas : Canvas/label pour l'affichage
 * 
 * @author Aurélien Conil
 * @date 2025
 */


// PdGuiObject.h
#pragma once

#include "ofMain.h"
#include <functional>

/// Types d'objets GUI supportés par le système
/// Utilisé pour l'identification du type d'objet et le casting sécurisé
enum class GuiType {
    HORIZONTAL_SLIDER,  ///< Slider horizontal (hsl dans .pd)
    VERTICAL_SLIDER,    ///< Slider vertical (vsl dans .pd)
    TOGGLE,            ///< Toggle/checkbox (tgl dans .pd)
    BANG,              ///< Bang/bouton (bng dans .pd)  
    NUMBER_BOX,        ///< Boîte de nombre (floatatom dans .pd)
    UNKNOWN            ///< Type non reconnu ou erreur
};

/// Structure pour la gestion optimisée des mises à jour FBO
/// Permet de ne redessiner que les zones modifiées pour de meilleures performances
struct GuiUpdateRegion {
    ofRectangle rect;      ///< Zone qui doit être redessinée
    bool needsUpdate;      ///< Flag indiquant si une mise à jour est nécessaire
    
    GuiUpdateRegion() : needsUpdate(false) {}
    GuiUpdateRegion(ofRectangle r) : rect(r), needsUpdate(true) {}
};

/**
 * @class PdGuiObject  
 * @brief Classe de base abstraite pour tous les objets GUI Pure Data
 * 
 * Cette classe définit l'interface commune et fournit l'infrastructure de base
 * pour tous les objets GUI Pure Data. Elle gère les aspects communs comme
 * les événements souris, les callbacks, l'optimisation FBO et les propriétés de base.
 * 
 * ARCHITECTURE :
 * - Interface abstraite : Les classes dérivées doivent implémenter update() et draw()
 * - Gestion automatique des événements souris avec hit testing
 * - Système de callbacks configurables pour l'intégration Pure Data
 * - Support de l'optimisation FBO avec zones de mise à jour
 * 
 * UTILISATION :
 * 1. Hériter de cette classe pour créer un nouvel objet GUI
 * 2. Implémenter update() et draw() selon les besoins
 * 3. Configurer les callbacks pour l'intégration Pure Data
 * 4. Utiliser markForUpdate() pour optimiser le rendu FBO
 */
class PdGuiObject {
public:
    // === CONSTRUCTION ET DESTRUCTION ===
    /// Constructeur principal - initialise toutes les propriétés de base
    /// @param type Type d'objet GUI (TOGGLE, BANG, etc.)
    /// @param position Position (x,y) dans l'espace de rendu
    /// @param size Taille (largeur, hauteur) de l'objet
    /// @param sendSymbol Symbole Pure Data pour l'envoi (nom de l'objet émetteur)
    /// @param receiveSymbol Symbole Pure Data pour la réception (nom de l'objet récepteur)
    PdGuiObject(GuiType type, ofVec2f position, ofVec2f size,
                const string& sendSymbol, const string& receiveSymbol);
    
    /// Destructeur virtuel - nécessaire pour l'héritage polymorphe
    virtual ~PdGuiObject() = default;
    
    // === MÉTHODES ABSTRAITES PRINCIPALES ===
    /// Mise à jour logique de l'objet (à implémenter dans les classes dérivées)
    virtual void update() = 0;
    
    /// Rendu de l'objet (à implémenter dans les classes dérivées)  
    virtual void draw() = 0;
    
    /// Rendu optimisé dans un FBO - méthode finale non surchargeble
    virtual void drawToFbo(ofFbo& fbo) final;
    
    // === SYSTÈME D'ÉVÉNEMENTS SOURIS ===
    /// Gestion du clic souris - retourne true si l'événement est géré
    virtual bool onMousePressed(ofMouseEventArgs& args);
    
    /// Gestion du drag souris - pour les sliders et interactions continues
    virtual bool onMouseDragged(ofMouseEventArgs& args);
    
    /// Gestion du relâchement souris - finalise les interactions
    virtual bool onMouseReleased(ofMouseEventArgs& args);
    
    /// Gestion du mouvement souris - pour le hover et le feedback visuel
    virtual bool onMouseMoved(ofMouseEventArgs& args);
    
    // === GESTION DES VALEURS ===
    /// Définit la valeur de l'objet (0.0 à 1.0 typiquement)
    virtual void setValue(float value);
    
    /// Récupère la valeur actuelle de l'objet  
    virtual float getValue() const { return currentValue; }
    
    /// Définit la plage de valeurs autorisées
    virtual void setValueRange(float min, float max);
    
    // === SYSTÈME D'OPTIMISATION FBO ===
    /// Marque l'objet entier pour mise à jour lors du prochain rendu FBO
    void markForUpdate();
    
    /// Marque une zone spécifique pour mise à jour (optimisation avancée)
    void markForUpdate(ofRectangle region);
    
    /// Vérifie si l'objet a besoin d'être redessiné
    bool needsUpdate() const { return updateRegion.needsUpdate; }
    
    /// Récupère la zone qui doit être redessinée
    ofRectangle getUpdateRegion() const { return updateRegion.rect; }
    
    /// Marque l'objet comme mis à jour (appelé après rendu FBO)
    void clearUpdateFlag() { updateRegion.needsUpdate = false; }
    
    // === ACCESSEURS ET PROPRIÉTÉS ===
    /// Type de l'objet GUI (pour identification et casting)
    GuiType getType() const { return type; }
    
    /// Position de l'objet dans l'espace de rendu
    ofVec2f getPosition() const { return position; }
    
    /// Taille de l'objet
    ofVec2f getSize() const { return size; }
    
    /// Rectangle englobant pour hit testing et rendu
    ofRectangle getBounds() const { return ofRectangle(position.x, position.y, size.x, size.y); }
    
    /// Symbole Pure Data pour l'envoi de valeurs
    string getSendSymbol() const { return sendSymbol; }
    
    /// Symbole Pure Data pour la réception de valeurs  
    string getReceiveSymbol() const { return receiveSymbol; }
    
    /// État de visibilité de l'objet
    bool isVisible() const { return visible; }
    void setVisible(bool v) { visible = v; if(v) markForUpdate(); }
    
    /// État d'activation de l'objet (peut-il réagir aux événements)
    bool isEnabled() const { return enabled; }
    void setEnabled(bool e) { enabled = e; markForUpdate(); }
    
    // === HIT TESTING ===
    /// Test de collision point/objet pour les événements souris
    bool isPointInside(ofVec2f point) const;
    bool isPointInside(float x, float y) const;
    
    // === CALLBACKS PURE DATA ===
    /// Callback appelé pour envoyer une valeur numérique vers Pure Data
    /// Configuré dans ofApp::setupCallbacks() pour utiliser ofxPd
    std::function<void(const string&, float)> onSendToPd;
    
    /// Callback appelé pour envoyer un message texte vers Pure Data  
    /// Configuré dans ofApp::setupCallbacks() pour utiliser ofxPd
    std::function<void(const string&, const string&)> onSendToPdString;
    
protected:
    // === PROPRIÉTÉS DE BASE ===
    GuiType type;                ///< Type d'objet GUI
    ofVec2f position;            ///< Position (x,y) dans l'espace de rendu
    ofVec2f size;                ///< Taille (largeur, hauteur)
    string sendSymbol;           ///< Symbole Pure Data pour l'envoi
    string receiveSymbol;        ///< Symbole Pure Data pour la réception
    
    // === VALEURS ET PLAGES ===
    float currentValue;          ///< Valeur actuelle de l'objet (0.0-1.0 typiquement)
    float minValue;              ///< Valeur minimale autorisée
    float maxValue;              ///< Valeur maximale autorisée
    
    // === ÉTATS D'INTERACTION ===
    bool visible;                ///< Objet visible à l'écran
    bool enabled;                ///< Objet peut réagir aux événements
    bool mouseOver;              ///< Souris au-dessus de l'objet
    bool mousePressed;           ///< Bouton souris enfoncé sur l'objet
    bool isDragging;             ///< Opération de drag en cours
    
    // === SYSTÈME FBO ===
    GuiUpdateRegion updateRegion; ///< Zone nécessitant une mise à jour
    
    // === GESTION SOURIS ===
    ofVec2f lastMousePos;        ///< Dernière position de la souris
    ofVec2f mousePressPos;       ///< Position lors du clic initial
    
    // === MÉTHODES UTILITAIRES PROTÉGÉES ===
    /// Envoie une valeur numérique vers Pure Data via le callback
    void sendToPd(float value);
    
    /// Envoie un message texte vers Pure Data via le callback
    void sendToPd(const string& message);
    
    /// Conversion coordonnées globales vers locales à l'objet
    ofVec2f globalToLocal(ofVec2f globalPos) const;
    
    /// Conversion coordonnées locales vers globales
    ofVec2f localToGlobal(ofVec2f localPos) const;
    
    // === MÉTHODES DE DESSIN DE BASE ===
    /// Dessine l'arrière-plan de l'objet (peut être surchargée)
    virtual void drawBackground();
    
    /// Dessine la bordure de l'objet (peut être surchargée)
    virtual void drawBorder();
    
    /// Dessine le label/texte de l'objet (peut être surchargée)
    virtual void drawLabel();
    
    // === PALETTE DE COULEURS STANDARD ===
    static const ofColor DEFAULT_BG_COLOR;      ///< Couleur d'arrière-plan par défaut
    static const ofColor DEFAULT_FG_COLOR;      ///< Couleur de premier plan par défaut
    static const ofColor DEFAULT_BORDER_COLOR;  ///< Couleur de bordure par défaut
    static const ofColor HOVER_COLOR;           ///< Couleur lors du survol
    static const ofColor PRESSED_COLOR;         ///< Couleur lors du clic
    
private:
    // === MÉTHODES PRIVÉES ===
    /// Met à jour les états de survol et de pression selon la position de la souris
    void updateMouseState(ofVec2f mousePos);
};
