//
//  PatchParser.h
//  pd-gui
//
//  Created by Aurélien Conil on 15/07/2025.
//

#pragma once

#include "ofMain.h"
#include "PdGuiObject.h"
#include "Toggle.h"
#include "Bang.h"
#include "Slider.h"
#include "NumberBox.h"
#include "Canvas.h"
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>

class PdPatchParser {
public:
    // Méthode principale pour parser un fichier patch
    std::vector<std::unique_ptr<PdGuiObject>> parseFile(const std::string& filename);
    
private:
    // Méthodes privées pour le parsing
    std::unique_ptr<PdGuiObject> parseLine(const std::string& line);
    std::unique_ptr<PdGuiObject> parseHorizontalSlider(const std::vector<std::string>& tokens, ofVec2f pos);
    std::unique_ptr<PdGuiObject> parseVerticalSlider(const std::vector<std::string>& tokens, ofVec2f pos);
    std::unique_ptr<PdGuiObject> parseToggle(const std::vector<std::string>& tokens, ofVec2f pos);
    std::unique_ptr<PdGuiObject> parseBang(const std::vector<std::string>& tokens, ofVec2f pos);
    std::unique_ptr<PdGuiObject> parseCanvas(const vector<string>& tokens, ofVec2f pos);
    std::unique_ptr<PdGuiObject> parseNumberBox(const std::vector<std::string>& tokens, ofVec2f pos);
    
    // Utilitaire pour diviser une chaîne
    std::vector<std::string> splitString(const std::string& str, char delimiter);
    
    ofColor parseHexColor(const string& hexStr);
};
