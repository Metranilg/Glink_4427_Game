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

    void update();          // �������� ImGui
    bool isLevelSelected() const;
    int getSelectedLevel() const;
    GameState getState() const;

    void pause();           // ��������� �� �����
    void resume();          // ��������� � ����

private:
    GameState state;
    int selectedLevel;
    bool levelSelected;
};
