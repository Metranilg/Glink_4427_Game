#pragma once
#include <imgui.h>

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAMEOVER
};

class Menu {
public:
    Menu();

    void update();          // Рендерит ImGui
    bool isLevelSelected() const;
    int getSelectedLevel() const;
    GameState getState() const;

    void pause();           // Поставить на паузу
    void resume();          // Вернуться к игре

private:
    GameState state;
    int selectedLevel;
    bool levelSelected;
};
