/**
 * @file NumberBox.h
 * @brief Implémentation des Number Box Pure Data (objet 'floatatom')
 * 
 * RÔLE DANS L'ARCHITECTURE :
 * Le PdNumberBox représente l'objet 'floatatom' de Pure Data - une boîte de saisie/affichage
 * numérique permettant d'entrer et d'afficher des valeurs numériques avec précision contrôlée.
 * 
 * FONCTIONNALITÉS :
 * - Affichage numérique avec précision configurable
 * - Modification de valeur par drag vertical (standard Pure Data)
 * - Plage de valeurs configurable (min/max)
 * - Sensibilité de drag ajustable
 * - Format d'affichage automatique selon la précision
 * - Intégration avec le système de callbacks pour ofxPd
 * 
 * INTERACTION PURE DATA :
 * - Drag vertical : Augmente (haut) ou diminue (bas) la valeur
 * - Sensibilité : Contrôle la rapidité de changement
 * - Maintien du comportement natif Pure Data
 * 
 * AFFICHAGE :
 * - Rectangle avec bordure et arrière-plan
 * - Texte centré avec formatage selon la précision
 * - Style cohérent avec les autres objets GUI
 * 
 * FORMAT PURE DATA :
 * #X floatatom x y width min max label_pos label font_size bg_color fg_color label_color...
 * 
 * @author Aurélien Conil
 * @date 2025
 */

//
//  NumberBox.h
//  pd-gui
//
//  Created by Aurélien Conil on 16/07/2025.
//

/**
 * @class PdNumberBox
 * @brief Implémentation de l'objet Number Box Pure Data
 * 
 * La number box est un élément d'interface essentiel de Pure Data pour
 * l'affichage et la saisie de valeurs numériques précises.
 * 
 * COMPORTEMENT DE DRAG :
 * - Drag vertical uniquement (comme Pure Data)
 * - Drag vers le haut : augmente la valeur
 * - Drag vers le bas : diminue la valeur
 * - Sensibilité configurable pour contrôler la rapidité
 * 
 * FORMATAGE :
 * - Précision décimale configurable (par défaut: 2)
 * - Formatage automatique selon la précision
 * - Gestion des entiers (précision = 0)
 * 
 * PLAGE DE VALEURS :
 * - Par défaut : -1000000.0 à 1000000.0 (large plage Pure Data)
 * - Configurable via setValueRange()
 * - Contrainte automatique dans la plage lors du drag
 */
class PdNumberBox : public PdGuiObject {
public:
    // === CONSTRUCTION ===
    /// Constructeur de la number box Pure Data
    /// @param position Position (x,y) dans l'espace de rendu
    /// @param size Taille (largeur, hauteur) de la boîte
    /// @param sendSymbol Symbole Pure Data pour l'envoi de valeurs
    /// @param receiveSymbol Symbole Pure Data pour la réception de valeurs
    /// @param min Valeur minimale autorisée (défaut: -1000000.0)
    /// @param max Valeur maximale autorisée (défaut: 1000000.0)
    /// @param initialValue Valeur initiale (défaut: 0.0)
    /// @param precision Nombre de décimales à afficher (défaut: 2)
    PdNumberBox(ofVec2f position, ofVec2f size,
                const string& sendSymbol, const string& receiveSymbol,
                float min = -1000000.0f, float max = 1000000.0f,
                float initialValue = 0.0f, int precision = 2);
    
    // === MÉTHODES VIRTUELLES HÉRITÉES ===
    /// Mise à jour logique (actuellement vide)
    virtual void update() override;
    
    /// Rendu de la number box avec texte formaté
    virtual void draw() override;
    
    // === GESTION DES ÉVÉNEMENTS SOURIS ===
    /// Gestion du clic - début du drag vertical
    virtual bool onMousePressed(ofMouseEventArgs& args) override;
    
    /// Gestion du drag - modification de la valeur selon le mouvement vertical
    virtual bool onMouseDragged(ofMouseEventArgs& args) override;
    
    /// Gestion du relâchement - fin du drag
    virtual bool onMouseReleased(ofMouseEventArgs& args) override;
    
    /// Gestion du mouvement - feedback visuel de survol
    virtual bool onMouseMoved(ofMouseEventArgs& args) override;
    
    // === MÉTHODES SPÉCIFIQUES À LA NUMBER BOX ===
    /// Définit la valeur numérique directement
    void setNumberValue(float value);
    
    /// Récupère la valeur numérique actuelle
    float getNumberValue() const;
    
    /// Configure la précision d'affichage (nombre de décimales)
    void setPrecision(int precision);
    
    /// Récupère la précision actuelle
    int getPrecision() const { return displayPrecision; }
    
    // === CONFIGURATION ===
    /// Configure la sensibilité du drag (rapidité de changement)
    void setDragSensitivity(float sensitivity);
    
    /// Configure la plage de valeurs autorisées
    void setValueRange(float min, float max);
    
protected:
    // === PROPRIÉTÉS D'AFFICHAGE ===
    int displayPrecision;        ///< Nombre de décimales à afficher
    float dragSensitivity;       ///< Sensibilité du drag (pixels → valeur)
    float dragStartValue;        ///< Valeur au début du drag
    float dragStartY;            ///< Position Y au début du drag
    bool isDraggingValue;        ///< Drag de valeur en cours
    
    // === FORMATAGE ET AFFICHAGE ===
    /// Formate la valeur selon la précision configurée
    string formatValue() const;
    
    /// Dessine le texte numérique centré
    void drawNumberText();
    
    /// Surcharge l'arrière-plan pour le style number box
    void drawBackground() override;
    
    // === CALCULS DE DRAG ===
    /// Calcule le delta de valeur selon le mouvement vertical
    float calculateDragDelta(float currentY) const;
    
    /// Met à jour la valeur selon le delta de drag
    void updateValueFromDrag(float deltaY);
    
private:
    // === CONSTANTES ===
    static const float DEFAULT_DRAG_SENSITIVITY; ///< Sensibilité par défaut du drag
    static const int DEFAULT_FONT_SIZE;          ///< Taille de police par défaut
};
