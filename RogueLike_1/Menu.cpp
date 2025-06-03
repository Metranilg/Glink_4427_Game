#include "Menu.h"
#include <string>
#include <iostream>

Menu::Menu() : state(GameState::MENU), selectedLevel(0), levelSelected(false) {}

void Menu::update() {
    if (state == GameState::MENU) {
        ImGui::Begin("Select Level", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::Text("Choose a level:");

        for (int i = 1; i <= 7; ++i) {
            std::string label = "Level " + std::to_string(i);
            if (ImGui::Button(label.c_str(), ImVec2(200, 40))) {
                selectedLevel = i;
                levelSelected = true;
                state = GameState::PLAYING;
            }
        }

        ImGui::End();
    }

    if (state == GameState::PAUSED) {
        ImGui::Begin("Paused", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Game is paused");

        if (ImGui::Button("Resume")) {
            resume();
        }

        ImGui::End();
    }
}

bool Menu::isLevelSelected() const {
    return levelSelected;
}

int Menu::getSelectedLevel() const {
    return selectedLevel;
}

GameState Menu::getState() const {
    return state;
}

void Menu::pause() {
    state = GameState::PAUSED;
}

void Menu::resume() {
    state = GameState::PLAYING;
}
