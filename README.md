# ofxPdGui - Visualisateur Pure Data Autonome

## Objectif du Projet

**ofxPdGui** est une application autonome développée avec openFrameworks qui permet de **lire et afficher des patches Pure Data sans dépendre du logiciel Pure Data**. L'objectif est de recréer visuellement l'interface graphique des patches .pd avec une fidélité maximale au style Pure Data original.

## Architecture du Projet

L'architecture d'ofxPdGui se compose de **deux parties principales** :

### 1. PARSER/DECODER 
**Responsable :** Conversion des fichiers .pd en objets C++ utilisables

- **`PatchParser.h/cpp`** : Système principal de parsing des fichiers .pd
- **Formats supportés :** 
  - `#X obj x y hsl/vsl ...` : Sliders horizontaux/verticaux
  - `#X obj x y tgl ...` : Toggles/checkboxes 
  - `#X obj x y bng ...` : Bangs/boutons momentanés
  - `#X obj x y cnv ...` : Canvas/labels
  - `#X floatatom x y ...` : Boîtes de nombre
- **Fonctionnalités :**
  - Lecture ligne par ligne des fichiers .pd
  - Extraction des paramètres (position, taille, couleurs, symboles)
  - Gestion des erreurs et parsing robuste
  - Création automatique des objets C++ correspondants

### 2. RENDERER
**Responsable :** Recréation visuelle et gestion des interactions

- **`ofApp.h/cpp`** : Classe principale, gestionnaire du rendu et des événements
- **`PdGuiObject.h/cpp`** : Classe de base abstraite pour tous les objets GUI
- **Classes GUI individuelles :**
  - `Toggle.h/cpp` : Implémentation des toggles (objet `tgl`)
  - `Bang.h/cpp` : Implémentation des bangs (objet `bng`)
  - `Slider.h/cpp` : Implémentation des sliders (objets `hsl`/`vsl`)
  - `NumberBox.h/cpp` : Implémentation des boîtes de nombre (`floatatom`)
  - `Canvas.h/cpp` : Implémentation des canvas/labels (objet `cnv`)

## État Actuel du Projet (2025)

### ✅ Fonctionnalités Implémentées

- **Objets GUI supportés :** bang, toggle, canvas, number box, sliders (horizontal/vertical)
- **Système de parsing complet** pour les fichiers .pd
- **Rendu fidèle** au style Pure Data original
- **Gestion complète des événements** souris et clavier
- **Système d'optimisation FBO** pour les performances
- **Architecture de callbacks** prête pour l'intégration ofxPd
- **Gestion des couleurs** Pure Data (parsing des couleurs hexadécimales)
- **Hit testing** et interactions précises

### 🎮 Contrôles Disponibles

- **Clic souris :** Activation des objets (toggles, bangs, etc.)
- **Drag souris :** Contrôle des sliders et number boxes
- **Raccourcis clavier :**
  - `r` : Reset de tous les objets
  - `a` : Activation de tous les objets
  - `t` : Toggle aléatoire

## Système d'Optimisation FBO

Le projet intègre un **système avancé d'optimisation de rendu** :

- **Frame Buffer Object (FBO)** : Évite de redessiner tous les objets à chaque frame
- **Rendu sélectif** : Seuls les objets modifiés sont redessinés
- **Zones de mise à jour** : Suivi précis des régions nécessitant un rendu
- **Performance** : Permet de gérer des patches complexes avec de nombreux objets

## Interface Pure Data (ofxPd) - Prête

L'architecture est **prête pour l'intégration complète avec ofxPd** :

```cpp
// Callbacks configurés dans ofApp::setupCallbacks()
obj->onSendToPd = [this](const string& symbol, float value) {
    // Actuellement : ofLogNotice("PD Send") << symbol << " = " << value;
    // À intégrer : ofxPd::sendFloat(symbol, value);
};

obj->onSendToPdString = [this](const string& symbol, const string& message) {
    // Actuellement : ofLogNotice("PD Send String") << symbol << " = " << message;
    // À intégrer : ofxPd::sendSymbol(symbol, message);
};
```

## Utilisation

### 1. Préparation
```bash
# Placer votre fichier .pd dans le dossier data
cp votre_patch.pd bin/data/patch.pd
```

### 2. Configuration (optionnelle)
```cpp
// Dans ofApp::setup(), modifier le nom du fichier si nécessaire
PdPatchParser parser;
guiObjects = parser.parseFile("votre_patch.pd");
```

### 3. Compilation et Exécution
```bash
make
./bin/ofxPdGui
```

## Roadmap Future

### 🔄 Prochaines Étapes

1. **Intégration ofxPd complète**
   - Remplacement des logs par les appels ofxPd réels
   - Gestion bidirectionnelle (réception depuis Pure Data)
   - Synchronisation audio/GUI

2. **Support des sous-patches**
   - Parsing des objets `#N canvas` (subpatches)
   - Navigation dans les sous-patches
   - Gestion hiérarchique des objets

3. **Support des externals**
   - Détection et support des externals avec GUI
   - Plugin system pour nouveaux objets
   - API d'extension pour développeurs

4. **Fonctionnalités avancées**
   - Système de sauvegarde des états
   - Mode plein écran et multi-monitor
   - Thèmes et personnalisation visuelle
   - Export/import de configurations

## Structure du Code

```
src/
├── main.cpp              # Point d'entrée principal avec documentation globale
├── ofApp.h/cpp           # Classe principale - gestionnaire du rendu
├── PatchParser.h/cpp     # Système de parsing des fichiers .pd
├── PdGuiObject.h/cpp     # Classe de base pour tous les objets GUI
├── Toggle.h/cpp          # Implémentation des toggles Pure Data
├── Bang.h/cpp            # Implémentation des bangs Pure Data  
├── Slider.h/cpp          # Implémentation des sliders Pure Data
├── NumberBox.h/cpp       # Implémentation des number boxes Pure Data
└── Canvas.h/cpp          # Implémentation des canvas Pure Data
```

## Exemple de Patch Supporté

Le projet inclut un fichier d'exemple `bin/data/patch.pd` contenant :
- Bangs de différentes tailles avec couleurs personnalisées
- Toggles avec symboles send/receive configurés
- Sliders horizontaux et verticaux avec plages de valeurs
- Number box avec précision configurable
- Canvas avec labels colorés

## Contribution

Ce projet constitue une base solide pour :
- **Étudiants** : Comprendre l'architecture Pure Data et openFrameworks
- **Développeurs** : Étendre le support à de nouveaux objets GUI
- **Musiciens/Artistes** : Créer des interfaces visuelles pour leurs patches Pure Data
- **Chercheurs** : Analyser et visualiser des patches Pure Data complexes

---

**Auteur :** Aurélien Conil  
**Date :** 2025  
**Framework :** openFrameworks  
**Licence :** [À spécifier]