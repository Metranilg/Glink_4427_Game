#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <sstream>

#include "stb_image.h"
#include "Menu.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Player.h"
#include "Enemy.h"
#include "LevelManager.h"

// Размер окна
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void updateWindowTitle(GLFWwindow* window, float elapsed, float health) {
    std::stringstream ss;
    ss << "Survival Time: " << static_cast<int>(elapsed) << "s | HP: " << static_cast<int>(health);
    glfwSetWindowTitle(window, ss.str().c_str());
}

// Функция столкновения (прототип, который вызывают Enemy и LevelManager)
bool isColliding(glm::vec2 a, float aSize, glm::vec2 b, float bSize) {
    return std::abs(a.x - b.x) < (aSize + bSize) &&
        std::abs(a.y - b.y) < (aSize + bSize);
}

// Глобальные объекты
Player       player;
Menu         menu;
LevelManager levelManager;
float        startTime = 0.0f;

void restartGame() {
    // Сброс состояния меню, игрока и менеджера уровней
    menu = Menu();
    player.reset();
    player.setScreenSize(SCR_WIDTH, SCR_HEIGHT);
    player.loadTexture("player.png");
    player.setScale(2.0f);
    player.setHealth(100.0f);
    player.setMaxHealth(100.0f);
    player.setExperience(0.0f);
    player.setLevel(1);

    levelManager.reset();
    levelManager.setPlayer(&player);

    startTime = static_cast<float>(glfwGetTime());
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Вычисляем сторону квадрата
    int size = std::min(width, height);

    // Вычисляем отступы (для центрирования)
    int xOffset = (width - size) / 2;
    int yOffset = (height - size) / 2;

    // Устанавливаем viewport
    glViewport(xOffset, yOffset, size, size);

    // Устанавливаем проекцию на квадратную область [-1,1] x [-1,1]
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);  // фиксированная игровая зона
    glMatrixMode(GL_MODELVIEW);
}


int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    // Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Roguelike", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Инициализация GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n";
        return -1;
    }

    // Настройка OpenGL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // Инициализация ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Первичное создание игрока и уровня
    restartGame();

    float lastFrameTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        // === ImGui: старт нового кадра ===
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // === Логика меню и игры ===
        if (menu.getState() != GameState::PLAYING) {
            // Рисуем главное меню или меню паузы
            menu.update();

            // Если выбрали уровень (0..6)
            if (menu.isLevelSelected()) {
                levelManager.loadLevel(menu.getSelectedLevel());
                menu.resume();
                startTime = static_cast<float>(glfwGetTime());
            }
        }
        else {
            // === Игровая логика ===
            player.update(deltaTime);
            player.updateAttack(deltaTime);

            levelManager.update(deltaTime);

            // Проверка смерти
            if (player.getHealth() <= 0.0f) {
                std::cout << "Game Over! You survived for "
                    << static_cast<int>(currentTime - startTime)
                    << " seconds.\n";

                // Если это уровень босса — выходим из игры
                if (levelManager.getCurrentLevel() == 7) {
                    std::cout << "Boss level failed. Exiting game.\n";
                    glfwSetWindowShouldClose(window, true);
                }
                else {
                    restartGame();
                }
            }


            // Пауза по ESC
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                menu.pause();
                if (levelManager.getCurrentLevel() == 7) {
                    std::cout << "Boss level failed. Exiting game.\n";
                    glfwSetWindowShouldClose(window, true);
                }
            }
        }

        // === ОТРИСОВКА ===
        glClear(GL_COLOR_BUFFER_BIT);

        if (menu.getState() == GameState::PLAYING) {
            levelManager.draw();
            updateWindowTitle(window, currentTime - startTime, player.getHealth());
            if (player.isAwaitingUpgrade()) {
                ImGui::Begin("Level Up! Choose an upgrade", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
                for (int choice : player.getUpgradeChoices()) {
                    const char* label = "";
                    switch (choice) {
                    case 0: label = "Speed"; break;
                    case 1: label = "Attack Speed"; break;
                    case 2: label = "Damage"; break;
                    case 3: label = "Max Health"; break;
                    case 4: label = "Health Regen"; break;
                    }
                    if (ImGui::Button(label)) {
                        player.applyUpgrade(choice);
                        player.setAwaitingUpgrade(false);
                    }
                }
                ImGui::End();
            }
            else {
                // Обычный HUD
                ImGui::SetNextWindowPos(ImVec2(10, 10));
                ImGui::SetNextWindowBgAlpha(0.3f);
                ImGui::Begin("HUD", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::Text("Time: %.1f s", currentTime - startTime);
                ImGui::Text("Level: %d", player.getLevel());
                ImGui::Text("HP: %.0f / %.0f", player.getHealth(), player.getMaxHealth());
                ImGui::Text("XP: %d / %d", (int)player.getExperience(), (int)player.getExperienceToNextLevel());
                ImGui::Text("Skill cooldown: %.1f", player.getSkillCooldown());
                ImGui::Text("Skill uses: %d / 15", player.getSpecialAbilityUses());
                ImGui::End();
            }
        }

        if (menu.getState() == GameState::PLAYING) {
            levelManager.draw();
            updateWindowTitle(window, currentTime - startTime, player.getHealth());
        }

        // Рендер ImGui поверх сцены
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Свап и опрос событий
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Ограничение FPS ≈ 60
        float frameTime = static_cast<float>(glfwGetTime()) - currentTime;
        float frameDelay = 1.0f / 60.0f;
        if (frameTime < frameDelay) {
            glfwWaitEventsTimeout(frameDelay - frameTime);
        }
    }

    // Очистка ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
