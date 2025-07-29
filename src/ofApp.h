/**
 * @file ofApp.h
 * @brief Classe principale de ofxPdGui - Renderer et gestionnaire de l'interface graphique
 * 
 * ARCHITECTURE GLOBALE DE ofxPdGui :
 * 
 * ofxPdGui est une application autonome qui permet de lire et afficher des patches Pure Data
 * sans dépendre du logiciel Pure Data lui-même. L'architecture se compose de deux parties principales :
 * 
 * 1. PARSER/DECODER (PatchParser) :
 *    - Analyse des fichiers .pd pour extraire la structure des objets GUI
 *    - Conversion des paramètres Pure Data vers des objets C++ utilisables
 *    - Support des objets GUI : bang, toggle, canvas, number, slider
 * 
 * 2. RENDERER (ofApp + classes GUI) :
 *    - Recréation visuelle fidèle des objets GUI Pure Data
 *    - Gestion des événements souris et clavier
 *    - Système d'optimisation FBO pour les performances
 *    - Interface de callback prête pour l'intégration ofxPd
 * 
 * ÉTAT ACTUEL DU PROJET :
 * - ✅ Objets GUI implémentés : bang, toggle, canvas, number, slider
 * - ✅ Système de parsing des fichiers .pd
 * - ✅ Optimisations de rendu avec FBO
 * - ✅ Architecture de callbacks préparée
 * - 🔄 À venir : intégration des sous-patches et externals avec GUI
 * 
 * ROADMAP :
 * - Intégration complète avec ofxPd pour l'audio
 * - Support des sous-patches (subpatches)
 * - Support des externals avec interface GUI
 * - Système de sauvegarde des états
 * 
 * @author Aurélien Conil
 * @date 2025
 */

#pragma once

#include "ofMain.h"
#include "PdGuiObject.h"
#include "Toggle.h"
#include "Bang.h"
#include "PatchParser.h"
#include "Slider.h"
#include <memory>

/**
 * @class ofApp
 * @brief Classe principale de l'application ofxPdGui - Gestionnaire du rendu et des interactions
 * 
 * Cette classe est le cœur du système de rendu de ofxPdGui. Elle est responsable de :
 * 
 * RESPONSABILITÉS PRINCIPALES :
 * - Chargement et gestion des objets GUI parsés depuis les fichiers .pd
 * - Rendu optimisé des objets GUI avec système FBO
 * - Gestion des événements souris et clavier
 * - Interface avec le système de callbacks vers Pure Data (via ofxPd)
 * - Optimisation des performances de rendu
 * 
 * SYSTÈME FBO :
 * Le système utilise un Frame Buffer Object (FBO) pour optimiser le rendu :
 * - Rendu sélectif : seuls les objets modifiés sont redessinés
 * - Réduction des appels de dessin pour de meilleures performances
 * - Support des zones de mise à jour partielles
 * 
 * ARCHITECTURE DES ÉVÉNEMENTS :
 * - Propagation des événements souris vers les objets GUI
 * - Gestion centralisée des raccourcis clavier
 * - Système de callbacks prêt pour l'intégration ofxPd
 */
class ofApp : public ofBaseApp {
public:
    // === SYSTÈME DE MODES ===
    /// Énumération des modes de l'application
    enum class AppMode {
        EDIT_MODE,     ///< Mode édition : objets déplaçables, interactions figées
        USE_MODE       ///< Mode utilisation : interactions normales, objets fixes
    };

    // === MÉTHODES PRINCIPALES DU CYCLE DE VIE ===
    void setup() override;    ///< Initialisation : chargement du patch et configuration FBO
    void update() override;   ///< Mise à jour des objets GUI et logique de simulation
    void draw() override;     ///< Rendu principal avec optimisations FBO
    
    // === GESTION DES ÉVÉNEMENTS SOURIS ===
    // Ces méthodes propagent les événements vers les objets GUI appropriés
    void mousePressed(int x, int y, int button) override;  ///< Clic souris : activation des objets
    void mouseDragged(int x, int y, int button) override;  ///< Drag souris : sliders et interactions
    void mouseReleased(int x, int y, int button) override; ///< Relâchement : fin d'interactions
    void mouseMoved(int x, int y) override;                ///< Mouvement : hover et feedback visuel
    
    // === GESTION DES ÉVÉNEMENTS CLAVIER ===
    void keyPressed(int key) override; ///< Raccourcis : 'r'=reset, 'a'=tout activer, 't'=toggle aléatoire, 'e'=mode édition, 'u'=mode utilisation
    
    // === GESTION DES MODES ===
    /// Bascule entre mode édition et mode utilisation
    void toggleMode();
    
    /// Active le mode édition (objets déplaçables)
    void setEditMode();
    
    /// Active le mode utilisation (interactions normales)
    void setUseMode();
    
    /// Récupère le mode actuel
    AppMode getCurrentMode() const { return currentMode; }
    
    /// Vérifie si on est en mode édition
    bool isEditMode() const { return currentMode == AppMode::EDIT_MODE; }
    
    /// Méthode statique pour que les objets GUI puissent connaître le mode actuel
    static bool isGlobalEditMode() { return globalEditMode; }
    
private:
    // === DONNÉES PRINCIPALES ===
    /// Collection de tous les objets GUI chargés depuis le fichier .pd
    /// Utilise smart pointers pour une gestion mémoire sûre
    vector<unique_ptr<PdGuiObject>> guiObjects;
    
    // === SYSTÈME D'OPTIMISATION FBO ===
    /// Frame Buffer Object pour le rendu optimisé - évite de redessiner tout à chaque frame
    ofFbo guiFbo;
    /// Flag indiquant si le FBO doit être entièrement mis à jour
    bool fboNeedsUpdate = true;
    
    // === SIMULATION ET TESTS ===
    /// Compteur temporel pour les fonctionnalités de test et simulation
    float simulationTime = 0.0f;
    
    /// Mode actuel de l'application (démarre en mode édition)
    AppMode currentMode = AppMode::EDIT_MODE;
    
    /// Variable statique pour partager le mode avec tous les objets GUI
    static bool globalEditMode;
    
    // === MÉTHODES DE CONFIGURATION ===
    void createToggles();     ///< [LEGACY] Création manuelle d'objets pour les tests
    void setupCallbacks();    ///< Configuration des callbacks vers Pure Data (ofxPd)
    void setupFbo();          ///< Initialisation du système FBO
    
    // === MÉTHODES DE RENDU ===
    void drawGuiObjects();                    ///< Rendu direct des objets GUI (méthode actuelle)
    void drawGuiObjectsToFbo();              ///< [FUTUR] Rendu optimisé via FBO
    void drawGuiObjectsToFboWithScissor();   ///< [FUTUR] Rendu FBO avec découpage
    void drawGuiObjectsToFboOptimized();     ///< [FUTUR] Rendu FBO ultra-optimisé
    
    // === MÉTHODES UTILITAIRES ===
    void simulateAutomaticChanges(); ///< [TEST] Simulation de changements automatiques
    int countActiveToggles();        ///< Comptage des toggles actifs pour le debug
    void drawDebugInfo();            ///< Affichage des informations de débogage
    
    // === OPTIMISATIONS FBO (À VENIR) ===
    /// Fusion des rectangles adjacents pour réduire les zones de rendu
    vector<ofRectangle> mergeAdjacentRectangles(const vector<ofRectangle>& rectangles);
    /// Test d'adjacence entre deux rectangles
    bool areAdjacent(const ofRectangle& a, const ofRectangle& b);
};
