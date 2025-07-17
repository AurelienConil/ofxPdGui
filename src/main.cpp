/**
 * @file main.cpp
 * @brief Point d'entrée principal de l'application ofxPdGui
 * 
 * APPLICATION ofxPdGui - VISUALISATEUR PURE DATA AUTONOME
 * ========================================================
 * 
 * OBJECTIF GLOBAL :
 * Créer une application autonome capable de lire et afficher des patches Pure Data
 * sans dépendre du logiciel Pure Data lui-même. Cette application reconstruit
 * visuellement l'interface graphique des patches .pd avec une fidélité maximale.
 * 
 * ARCHITECTURE COMPLÈTE :
 * 
 * 1. **PARSER/DECODER** (PatchParser + classes associées)
 *    └─ Analyse des fichiers .pd pour extraire la structure GUI
 *    └─ Conversion syntaxe Pure Data → objets C++ utilisables  
 *    └─ Support : bang, toggle, canvas, number, slider
 * 
 * 2. **RENDERER** (ofApp + classes GUI + PdGuiObject)
 *    └─ Recréation visuelle fidèle des objets GUI Pure Data
 *    └─ Gestion des événements souris/clavier
 *    └─ Système d'optimisation FBO pour les performances
 *    └─ Architecture de callbacks prête pour ofxPd
 * 
 * ÉTAT ACTUEL (2025) :
 * ✅ Objets GUI implémentés : bang, toggle, canvas, number, slider
 * ✅ Parser complet pour fichiers .pd
 * ✅ Système de rendu optimisé avec FBO
 * ✅ Architecture de callbacks préparée pour ofxPd
 * ✅ Gestion complète des événements utilisateur
 * 
 * ROADMAP FUTURE :
 * 🔄 Intégration complète avec ofxPd pour l'audio
 * 🔄 Support des sous-patches (subpatches)  
 * 🔄 Support des externals avec interface GUI
 * 🔄 Système de sauvegarde des états
 * 🔄 Mode plein écran et multi-monitor
 * 
 * UTILISATION :
 * 1. Place ton fichier .pd dans bin/data/
 * 2. Modifie le nom dans ofApp::setup() si nécessaire
 * 3. Lance l'application pour visualiser ton patch
 * 
 * @author Aurélien Conil
 * @date 2025
 */

#include "ofMain.h"
#include "ofApp.h"

//========================================================================
/// Point d'entrée principal - Configuration de la fenêtre et lancement
int main( ){

	// === CONFIGURATION DE LA FENÊTRE ===
	// Utilise ofGLFWWindowSettings pour des options avancées (multi-monitor, etc.)
	ofGLWindowSettings settings;
	settings.setSize(1024, 768);        // Taille adaptée pour les patches Pure Data
	settings.windowMode = OF_WINDOW;    // Mode fenêtré (peut être OF_FULLSCREEN)

	// === CRÉATION ET LANCEMENT ===
	auto window = ofCreateWindow(settings);
	
	// Lance l'application avec notre classe principale ofApp
	ofRunApp(window, make_shared<ofApp>());
	ofRunMainLoop();

}
