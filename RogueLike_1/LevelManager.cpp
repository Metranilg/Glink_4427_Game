
#include "LevelManager.h"
//#include <SFML/Audio.hpp>
#include <GLFW/glfw3.h>   // Для glfwGetTime()
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include "stb_image.h"


// Вспомогательная функция столкновения
// Её прототип объявлен в main.cpp, поэтому используем extern:
extern bool isColliding(glm::vec2 a, float aSize, glm::vec2 b, float bSize);

/*
// -----------------------------------------------------------------------------
// Вспомогательный метод, который загружает музыку (если включён USE_SFML_AUDIO).
// Он вызывается из loadLevel().
void LevelManager::tryPlayMusic() {
#ifdef USE_SFML_AUDIO
    if (musicFile.empty()) return;
    if (!music.openFromFile(musicFile)) {
        std::cerr << "LevelManager: Не удалось загрузить музыку: " << musicFile << std::endl;
    }
    else {
        music.setLooping(true);
        music.play();
    }
#endif
}
*/

// -----------------------------------------------------------------------------
// Устанавливаем параметры (фон, enemyTypes, musicFile) в зависимости от номера уровня.
// id: 1..7 — если id выходит за пределы, получаем «пустой» уровень.
void LevelManager::initLevelData(int id) {
    switch (id) {

    case 1:
        backgroundColor = { 0.30f, 0.00f, 0.00f };
        enemyTypes = {
            {"demon1.png", 80.0f, 5.0f, 0.08f, 0.0035f, 4, 0.30f, 81, 71},   // медленный
            {"demon1.png", 100.0f, 7.0f, 0.10f, 0.005f, 4, 0.30f, 81, 71},   // средний
            {"demon1.png", 60.0f, 4.0f, 0.10f, 0.007f, 4, 0.30f, 81, 71}     // быстрый
        };
        musicFile = "music1lvl.ogg";
        break;

    case 2:
        backgroundColor = { 0.25f, 0.00f, 0.00f };
        enemyTypes = {
            {"demon1.png", 90.0f, 6.0f, 0.10f, 0.0035f, 4, 0.25f, 81, 71},
            {"demon1.png", 120.0f, 10.0f, 0.12f, 0.0055f, 4, 0.25f, 81, 71},
            {"demon1.png", 80.0f, 5.0f, 0.12f, 0.0075f, 4, 0.25f, 81, 71}
        };
        musicFile = "music2lvl.ogg";
        break;

    case 3:
        backgroundColor = { 0.20f, 0.00f, 0.00f };
        enemyTypes = {
            {"demon1.png", 110.0f, 7.0f, 0.12f, 0.004f, 4, 0.20f, 81, 71},
            {"demon1.png", 140.0f, 12.0f, 0.14f, 0.006f, 4, 0.20f, 81, 71},
            {"demon1.png", 100.0f, 6.0f, 0.14f, 0.0075f, 4, 0.20f, 81, 71}
        };
        musicFile = "music3lvl.ogg";
        break;

    case 4:
        backgroundColor = { 0.15f, 0.00f, 0.00f };
        enemyTypes = {
            {"demon1.png", 130.0f, 9.0f, 0.14f, 0.0045f, 4, 0.18f, 81, 71},
            {"demon1.png", 180.0f, 15.0f, 0.16f, 0.0065f, 4, 0.18f, 81, 71},
            {"demon1.png", 150.0f, 10.0f, 0.16f, 0.0075f, 4, 0.18f, 81, 71}
        };
        musicFile = "music4lvl.ogg";
        break;

    case 5:
        backgroundColor = { 0.10f, 0.00f, 0.00f };
        enemyTypes = {
            {"demon1.png", 150.0f, 11.0f, 0.16f, 0.005f, 4, 0.15f, 81, 71},
            {"demon1.png", 220.0f, 20.0f, 0.18f, 0.0065f, 4, 0.15f, 81, 71},
            {"demon1.png", 180.0f, 12.0f, 0.18f, 0.0075f, 4, 0.15f, 81, 71}
        };
        musicFile = "music5lvl.ogg";
        break;

    case 6:
        backgroundColor = { 0.05f, 0.00f, 0.00f };
        enemyTypes = {
            {"demon1.png", 170.0f, 12.0f, 0.16f, 0.0055f, 4, 0.13f, 81, 71},
            {"demon1.png", 250.0f, 22.0f, 0.18f, 0.0068f, 4, 0.13f, 81, 71},
            {"demon1.png", 200.0f, 15.0f, 0.18f, 0.0075f, 4, 0.13f, 81, 71}
        };
        musicFile = "music6lvl.ogg";
        break;

    case 7: {
        backgroundColor = { 0.05f, 0.05f, 0.2f };
        enemyTypes.clear();  // убираем обычных врагов
        musicFile = "music7lvl.ogg";
        bossActive = true;
        bossHealth = 150000000;
        bossPosition = glm::vec2(0.0f, 0.8f); //сверху посередине
        glGenTextures(1, &bossTextureID);
        glBindTexture(GL_TEXTURE_2D, bossTextureID);
        int width, height, nrChannels;
        unsigned char* data = stbi_load(bossTexture.c_str(), &width, &height, &nrChannels, 4);
        if (data) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else {
            std::cerr << "Failed to load boss texture" << std::endl;
        }
        stbi_image_free(data);
        break;
    }


    default:
        // Пустой уровень (фон серый, без врагов и музыки)
        backgroundColor = { 0.10f, 0.10f, 0.10f };
        enemyTypes.clear();
        musicFile.clear();
        break;
    }
}

// -----------------------------------------------------------------------------
// LevelManager::loadLevel(int id):
//  - Запоминает номер уровня
//  - Очищает вектор enemies
//  - Сбрасывает lastSpawnTime
//  - Запускает initLevelData(id), чтобы установить фон/врагов/музыку
//  - Вызывает tryPlayMusic() для воспроизведения трека
void LevelManager::loadLevel(int id) {
    currentLevel = id;
    enemies.clear();
    lastSpawnTime = 0.0f;

    levelStartTime = static_cast<float>(glfwGetTime());

    initLevelData(id);
    //tryPlayMusic();
}

// -----------------------------------------------------------------------------
// LevelManager::update(float deltaTime):
// 1) Спавн врагов через spawnInterval
// 2) Обновление движения и анимации каждого врага
// 3) Нанесение урона врагам, если активна зона атаки игрока
// 4) Столкновение «игрок ⇆ враг»: урон игроку + отталкивание врага
// 5) Удаление мёртвых врагов
void LevelManager::update(float deltaTime) {
    if (!player) return;

    float now = static_cast<float>(glfwGetTime());

    // Проверка победы (кроме 7 уровня)
    if (currentLevel != 7) {
        float elapsed = now - levelStartTime;
        if (elapsed >= 120.0f) {  // 2 минуты
            std::cout << "Victory! You survived 2 minutes!" << std::endl;
            glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
            return;
        }
    }

    // 1) Спавн врагов по краям игрового поля
    if (currentLevel != 7 && now - lastSpawnTime >= spawnInterval && !enemyTypes.empty()) {
        int side = rand() % 4;
        float ex, ey;
        switch (side) {
        case 0: // LEFT
            ex = -1.05f;
            ey = (rand() % 2000 - 1000) / 1000.0f;
            break;
        case 1: // RIGHT
            ex = 1.05f;
            ey = (rand() % 2000 - 1000) / 1000.0f;
            break;
        case 2: // TOP
            ex = (rand() % 2000 - 1000) / 1000.0f;
            ey = 1.05f;
            break;
        case 3: // BOTTOM
            ex = (rand() % 2000 - 1000) / 1000.0f;
            ey = -1.05f;
            break;
        }
        EnemyType& type = enemyTypes[rand() % enemyTypes.size()];
        enemies.emplace_back(ex, ey, type);
        lastSpawnTime = now;
    }

    // 2) Движение врагов
    glm::vec2 playerPos = player->getPosition();
    for (auto& e : enemies) {
        e.update(playerPos.x, playerPos.y, deltaTime);
    }

    // 3) Урон врагам от атаки игрока
    glm::vec2 zoneCenter = player->getDamageZoneCenter();
    float zoneSize = player->getDamageZoneSize();
    if (player->isAttackActive()) {
        for (auto& e : enemies) {
            if (isColliding(zoneCenter, zoneSize, { e.x, e.y }, e.size)) {
                e.health -= player->getAttackDamage();
            }
        }
    }

    // 4) Столкновение игрока и врагов
    float playerSize = player->getSize();
    playerDamageCooldown -= deltaTime;
    for (auto& e : enemies) {
        if (isColliding(playerPos, playerSize, { e.x, e.y }, e.size)) {
            // Урон игроку (не чаще чем раз в playerDamageInterval)
            if (playerDamageCooldown <= 0.0f) {
                player->takeDamage(e.damage);
                playerDamageCooldown = playerDamageInterval;
            }
            // Отталкиваем врага от игрока
            float dx = e.x - playerPos.x;
            float dy = e.y - playerPos.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist > 0.0f) {
                dx /= dist;
                dy /= dist;
                float pushStrength = 0.02f;
                e.x += dx * pushStrength;
                e.y += dy * pushStrength;
            }
        }
    }

    // 5) Удаляем мертвых
    for (auto it = enemies.begin(); it != enemies.end();) {
        if (it->health <= 0.0f) {
            player->addExperience(10);  // опыт за убийство
            it = enemies.erase(it);
        }
        else {
            ++it;
        }
    }

    if (currentLevel == 7 && bossActive) {
        // 1) Проверка победы
        if (player->getSpecialAbilityUses() >= 15) {
            bossActive = false; // Босс побежден
            std::cout << "Boss defeated! Congratulations!" << std::endl;
        }

        // 2) Спавн препятствий
        obstacleSpawnTimer += deltaTime;
        if (obstacleSpawnTimer >= obstacleSpawnInterval) {
            obstacleSpawnTimer = 0.0f;
            Obstacle o;
            o.position = glm::vec2((rand() % 2000 - 1000) / 1000.0f, 1.05f);
            o.direction = glm::normalize(glm::vec2((rand() % 2000 - 1000) / 1000.0f, -1.0f));
            o.speed = 0.5f;
            o.size = 0.05f;
            obstacles.push_back(o);
        }

        // 3) Движение препятствий
        for (auto& o : obstacles) {
            o.position += o.direction * o.speed * deltaTime;
        }

        // 4) Проверка столкновений с игроком
        for (auto& o : obstacles) {
            if (isColliding(player->getPosition(), player->getSize(), o.position, o.size)) {
                player->takeDamage(20.0f);
            }
        }

        // 5) Удаление ушедших препятствий
        obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(),
            [](const Obstacle& o) {
                return o.position.y < -1.2f || o.position.y > 1.2f ||
                    o.position.x < -1.2f || o.position.x > 1.2f;
            }), obstacles.end());
    }

    bossAnimationTimer += deltaTime;
    if (bossAnimationTimer >= bossFrameDuration) {
        bossAnimationTimer -= bossFrameDuration;
        bossCurrentFrame = (bossCurrentFrame + 1) % bossFrameCount;
    }
}

// -----------------------------------------------------------------------------
// LevelManager::draw()
//  - Заливает фон цветом backgroundColor
//  - Рисует игрока (player->draw()) и всех врагов (e.draw())
//  - Если активна атака игрока, рисует желтый квад (зону атаки)
void LevelManager::draw() {
    // 1) Чёрный фон всего окна
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 2) Отрисовка игрового поля
    glColor3f(backgroundColor.r, backgroundColor.g, backgroundColor.b);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // 3) Игрок и враги
    if (player) player->draw();
    for (auto& e : enemies) {
        e.draw();
    }

    // 4) Зона атаки
    if (player && player->isAttackActive()) {
        glm::vec2 dz = player->getDamageZoneCenter();
        float sz = player->getDamageZoneSize();
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(dz.x - sz, dz.y - sz);
        glVertex2f(dz.x + sz, dz.y - sz);
        glVertex2f(dz.x + sz, dz.y + sz);
        glVertex2f(dz.x - sz, dz.y + sz);
        glEnd();
    }

    /*
    // 5) Отрисовка босса
    if (currentLevel == 7 && bossActive) {
       // glColor3f(1.0f, 0.0f, 0.0f);  // Красный босс
        glBegin(GL_QUADS);
        glVertex2f(bossPosition.x - bossSize, bossPosition.y - bossSize);
        glVertex2f(bossPosition.x + bossSize, bossPosition.y - bossSize);
        glVertex2f(bossPosition.x + bossSize, bossPosition.y + bossSize);
        glVertex2f(bossPosition.x - bossSize, bossPosition.y + bossSize);
        glEnd();
    }
    */

    // 6) Отрисовка препятствий
    for (auto& o : obstacles) {
        glColor3f(1.0f, 0.0f, 0.0f);  // Красный цвет
        glBegin(GL_QUADS);
        glVertex2f(o.position.x - o.size, o.position.y - o.size);
        glVertex2f(o.position.x + o.size, o.position.y - o.size);
        glVertex2f(o.position.x + o.size, o.position.y + o.size);
        glVertex2f(o.position.x - o.size, o.position.y + o.size);
        glEnd();
    }

    if (bossActive) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, bossTextureID);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float texWidth = bossSpriteWidth * bossFrameCount;
        float u0 = bossCurrentFrame * bossSpriteWidth / texWidth;
        float u1 = (bossCurrentFrame + 1) * bossSpriteWidth / texWidth;

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
        glTexCoord2f(u0, 0.0f); glVertex2f(bossPosition.x - bossSize, bossPosition.y - bossSize);
        glTexCoord2f(u1, 0.0f); glVertex2f(bossPosition.x + bossSize, bossPosition.y - bossSize);
        glTexCoord2f(u1, 1.0f); glVertex2f(bossPosition.x + bossSize, bossPosition.y + bossSize);
        glTexCoord2f(u0, 1.0f); glVertex2f(bossPosition.x - bossSize, bossPosition.y + bossSize);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
}

// -----------------------------------------------------------------------------
// LevelManager::reset()
//  - Очищает вектор врагов
//  - Сбрасывает номер уровня
//  - Останавливает музыку (если USE_SFML_AUDIO)
void LevelManager::reset() {
    enemies.clear();
    currentLevel = -1;
    /*
#ifdef USE_SFML_AUDIO
    try {
        music.stop();
    }
    catch (...) {
        // музыка ещё не загружена
    }
#endif
*/
}
 