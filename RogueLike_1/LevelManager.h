#pragma once

#include <vector>
#include <string>

#include <Y:\game\glm-master\glm-master\glm\glm.hpp>     // для glm::vec3, glm::vec2

#include "Enemy.h"         // ваш класс Enemy и структура EnemyType
#include "Player.h"        // ваш класс Player

//#ifdef USE_SFML_AUDIO
//#include <SFML/Audio.hpp>
//#endif


struct Obstacle {
    glm::vec2 position;
    glm::vec2 direction;
    float speed;
    float size;
};

class LevelManager {
public:
    LevelManager() = default;
    ~LevelManager() = default;

    /// Передача указателя на игрока (нужно, чтобы LevelManager мог вызывать player->draw(), player->takeDamage() и т.д.)
    void setPlayer(Player* p) { player = p; }

    /// Загружает данные выбранного уровня (цвет фона, список типов врагов, файл музыки)
    /// @param id  Номер уровня: 1..7 (любые другие значения дадут пустой уровень с тёмным фоном)
    void loadLevel(int id);

    /// Обновляет логику: спавн врагов, их движение, анимацию, урон от игрока и столкновения «игрок⇆враг»
    /// @param deltaTime  Время (в секундах), прошедшее с предыдущего кадра
    void update(float deltaTime);

    /// Отрисовывает фон, игрока, врагов и (если активна) зону атаки игрока
    void draw();

    /// Полностью сбрасывает внутреннее состояние (удаляет всех врагов, останавливает музыку, сбрасывает номер уровня)
    void reset();

    int getCurrentLevel() const { return currentLevel; }


private:
    int                    currentLevel = -1;             // Текущий номер уровня
    float                  lastSpawnTime = 0.0f;           // Время последнего спавна врага
    float                  spawnInterval = 3.0f;           // Интервал спавна (сек)
    glm::vec3              backgroundColor{ 0.10f, 0.10f, 0.10f }; // Цвет фона

   // #ifdef USE_SFML_AUDIO
    //sf::Music music;
    //#endif

    std::vector<EnemyType> enemyTypes;                      // Шаблоны врагов для текущего уровня
    std::vector<Enemy>     enemies;                         // Живые экземпляры врагов

    std::string            musicFile;                       // Имя файла с музыкой уровня

    Player* player = nullptr;                // Указатель на объект игрока

    // Таймер, чтобы не наносить урон игроку каждую миллисекунду (если нужно замедлить)
    float                  playerDamageCooldown = 0.0f;
    float                  playerDamageInterval = 0.2f;     // Урон «враг⇆игрок» раз в 0.2 секунды

    // Вспомогательная функция загрузки музыки (вызывается внутри loadLevel):
    //void tryPlayMusic();

    // Вспомогательный метод, задающий данные для всех 7 уровней:
    void initLevelData(int id);

    glm::vec2 bossPosition = glm::vec2(0.0f, 0.9f);
    float bossSize = 0.4f;
    int bossHealth = 15;
    bool bossActive = false;

    // Препятствия
    std::vector<Obstacle> obstacles;
    float obstacleSpawnTimer = 0.0f;
    const float obstacleSpawnInterval = 0.1f;

    std::string bossTexture = "boss.png";  // путь к текстуре
    unsigned int bossTextureID = 0;
    int bossFrameCount = 15;
    float bossFrameDuration = 0.2f;
    int bossSpriteWidth = 224;  // подставь свой спрайт
    int bossSpriteHeight = 240;
    int bossCurrentFrame = 0;
    float bossAnimationTimer = 0.0f;

    float levelStartTime = 0.0f;

};
