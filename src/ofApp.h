#pragma once

#include "ofMain.h"
#include "PdGuiObject.h"
#include "Toggle.h"
#include "Bang.h"
#include "PatchParser.h"
#include "Slider.h"
#include <memory>

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    
    // Événements souris
    void mousePressed(int x, int y, int button) override;
    void mouseDragged(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void mouseMoved(int x, int y) override;
    
    // Événements clavier
    void keyPressed(int key) override;
    
private:
    // Vecteur de pointeurs vers les objets GUI
    vector<unique_ptr<PdGuiObject>> guiObjects;
    
    // FBO pour le rendu optimisé
    ofFbo guiFbo;
    bool fboNeedsUpdate = true;
    
    // Compteur pour la simulation
    float simulationTime = 0.0f;
    
    // Méthodes privées
    void createToggles();
    void setupCallbacks();
    void setupFbo();
    void drawGuiObjects();
    void drawGuiObjectsToFbo();
    void drawGuiObjectsToFboWithScissor();
    void drawGuiObjectsToFboOptimized();
    void simulateAutomaticChanges();
    int countActiveToggles();
    void drawDebugInfo();
    
    // Méthodes utilitaires pour l'optimisation FBO
    vector<ofRectangle> mergeAdjacentRectangles(const vector<ofRectangle>& rectangles);
    bool areAdjacent(const ofRectangle& a, const ofRectangle& b);
};
