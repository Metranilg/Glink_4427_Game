#ifndef PLAYER_H
#define PLAYER_H

#include <Y:\game\glm-master\glm-master\glm\glm.hpp>
#include <iostream>
#include <vector>

class Player {
public:
    Player();
    Player(const Player&) = delete;             // Запрещаем копирование
    Player& operator=(const Player&) = delete;  // Запрещаем присваивание

    void update(float deltaTime);
    void draw() const;
    void takeDamage(float dmg);
    float getHealth() const;
    float getMaxHealth() const;
    void setHealth(float value);
    void setMaxHealth(float value);

    glm::vec2 getPosition() const;
    float getSize() const;

    glm::vec2 getDirection() const;
    glm::vec2 getDamageZoneCenter() const;
    float getDamageZoneSize() const;

    void updateAttack(float deltaTime);
    bool isAttackActive() const;

    float getAttackDamage() const;
    void setAttackDamage(float dmg);

    unsigned int textureID;
    bool textureLoaded = false;

    bool loadTexture(const char* path);
    void setScale(float newScale);
    void setScreenSize(float width, float height);

    // XP/Level
    void addExperience(float xp);
    int getLevel() const;
    float getExperience() const;
    float getExperienceToNextLevel() const;
    void setExperience(float xp);
    void setLevel(int lvl);

    void reset();

    // Upgrade system
    void applyUpgrade(int choice);
    bool isAwaitingUpgrade() const;
    void setAwaitingUpgrade(bool state);
    void generateUpgradeChoices();

    // Доступ к upgradeChoices
    const std::vector<int>& getUpgradeChoices() const { return upgradeChoices; }

    void updateSkill(float deltaTime);
    bool isSkillReady() const;
    bool isSkillActive() const;
    float getSkillCooldown() const;

    int getSpecialAbilityUses() const { return specialAbilityUses; }
    void incrementSpecialAbilityUses() { ++specialAbilityUses; }



private:
    float attackCooldown = 2.0f;
    float attackTimer = 0.0f;
    float attackDuration = 0.3f;
    float attackActiveTime = 0.0f;

    float experience = 0.0f;
    int level = 1;
    const float experiencePerLevel = 100.0f;

    glm::vec2 position;
    glm::vec2 direction;
    float size;
    float speed;
    float health;
    float maxHealth;
    float attackDamage;
    float regenRate;
    float regenTimer;
    int textureWidth = 0;
    int textureHeight = 0;
    float scale = 1.0f;

    float screenWidth = 1.0f;
    float screenHeight = 1.0f;

    int speedUpgradeLevel = 0;
    int attackCooldownUpgradeLevel = 0;
    int damageUpgradeLevel = 0;
    int maxHealthUpgradeLevel = 0;
    int regenRateUpgradeLevel = 0;

    // Флаг выбора улучшения
    bool awaitingUpgradeChoice = false;

    std::vector<int> upgradeChoices;

    bool skillActive = false;
    float skillDuration = 1.5f;    // Длительность умения
    float skillTimer = 0.0f;       // Таймер активности умения
    float skillCooldown = 10.0f;   // Кулдаун
    float skillCooldownTimer = 0.0f; // Таймер кулдауна

    int specialAbilityUses = 0;
};

#endif // PLAYER_H
