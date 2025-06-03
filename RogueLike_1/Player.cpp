#define GLM_ENABLE_EXPERIMENTAL


#include "stb_image.h"
#include "Player.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <Y:\game\glm-master\glm-master\glm\gtx\norm.hpp>
#include <algorithm>  // для std::shuffle
#include <random>

Player::Player()
    : position(0.0f, 0.0f),
    direction(0.0f, 1.0f),
    size(0.05f),
    speed(0.5f),
    health(100.0f),
    maxHealth(100.0f),
    attackDamage(10.0f),
    regenRate(0.0f),
    regenTimer(0.0f)
{}

float screenWidth = 1.0f;  // по умолчанию
float screenHeight = 1.0f;

void Player::setScreenSize(float width, float height) {
    screenWidth = width;
    screenHeight = height;
}

bool Player::loadTexture(const char* path) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 4);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }

    textureWidth = width;
    textureHeight = height;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // для пиксель-арта

    stbi_image_free(data);
    textureLoaded = true;
    return true;
}

void Player::setScale(float newScale) {
    scale = newScale;
}


float Player::getAttackDamage() const {
    return attackDamage;
}

void Player::setAttackDamage(float dmg) {
    attackDamage = dmg;
}

void Player::updateAttack(float deltaTime) {
    attackTimer -= deltaTime;

    if (attackTimer <= 0.0f) {
        // Время для новой атаки
        attackTimer = attackCooldown;
        attackActiveTime = attackDuration;
    }

    if (attackActiveTime > 0.0f) {
        attackActiveTime -= deltaTime;
    }
}

bool Player::isAttackActive() const {
    return attackActiveTime > 0.0f;
}


void Player::update(float deltaTime) {
    GLFWwindow* window = glfwGetCurrentContext();
    glm::vec2 input(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) input.y += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) input.y -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) input.x -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) input.x += 1.0f;

    if (glm::length2(input) > 0.0f) {
        direction = glm::normalize(input);
        position += direction * speed * deltaTime;
    }



    updateSkill(deltaTime);

    // Ограничение границ
    float halfSize = size;
    if (position.x - halfSize < -1.0f) position.x = -1.0f + halfSize;
    if (position.x + halfSize > 1.0f) position.x = 1.0f - halfSize;
    if (position.y - halfSize < -1.0f) position.y = -1.0f + halfSize;
    if (position.y + halfSize > 1.0f) position.y = 1.0f - halfSize;

    regenTimer += deltaTime;
    if (regenTimer >= 1.0f) {
        health += regenRate;
        if (health > maxHealth) health = maxHealth;
        regenTimer = 0.0f;
    }
}

void Player::draw() const {
    float x = position.x;
    float y = position.y;

    if (textureLoaded) {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // белый цвет без окрашивания

        float halfWidth = 0.1f;
        float halfHeight = 0.1f;

        // OpenGL координаты [-1..1], позиция уже в world-space
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x - halfWidth, y - halfHeight);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x + halfWidth, y - halfHeight);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x + halfWidth, y + halfHeight);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x - halfWidth, y + halfHeight);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }
}

void Player::takeDamage(float dmg) {
    health -= dmg;
    if (health < 0.0f)
        health = 0.0f;
}

float Player::getHealth() const {
    return health;
}

glm::vec2 Player::getPosition() const {
    return position;
}

float Player::getSize() const {
    return size;
}

glm::vec2 Player::getDirection() const {
    return direction;
}

glm::vec2 Player::getDamageZoneCenter() const {
    return position + direction * (size + 0.05f);
}

float Player::getDamageZoneSize() const {
    float baseSize = size * 0.8f * 1.2f;  // +20% к зоне урона
    if (skillActive) {
        baseSize *= 10.0f;  // увеличение во время умения
    }
    return baseSize;
}


float Player::getExperience() const { return experience; }
int Player::getLevel() const { return level; }

void Player::addExperience(float amount) {
    experience += amount;
    while (experience >= experiencePerLevel) {
        experience -= experiencePerLevel;
        level++;
        awaitingUpgradeChoice = true;
        generateUpgradeChoices();
    }
}


void Player::setExperience(float xp) { experience = xp; }
void Player::setLevel(int lvl) { level = lvl; }

float Player::getExperienceToNextLevel() const {
    return experiencePerLevel;
}

float Player::getMaxHealth() const {
    return maxHealth;
}

void Player::setHealth(float value) {
    health = value;
    if (health > maxHealth) {
        health = maxHealth;
    }
}

void Player::setMaxHealth(float value) {
    maxHealth = value;
    if (health > maxHealth) {
        health = maxHealth;
    }
}

void Player::reset() {
    position = glm::vec2(0.0f, 0.0f);
    direction = glm::vec2(0.0f, 1.0f);
    size = 0.05f;
    speed = 0.5f;
    health = 100.0f;
    maxHealth = 100.0f;
    attackDamage = 5.0f;
    regenRate = 0.0f;
    regenTimer = 0.0f;
    experience = 0.0f;
    level = 1;
    specialAbilityUses = 0;
}

void Player::applyUpgrade(int choice) {
    switch (choice) {
    case 0: // Скорость
        if (speedUpgradeLevel < 6) {
            speed *= 1.2f;
            speedUpgradeLevel++;
        }
        break;
    case 1: // Скорость атаки (уменьшение cooldown)
        if (attackCooldownUpgradeLevel < 6) {
            attackCooldown /= 1.2f;
            attackCooldownUpgradeLevel++;
        }
        break;
    case 2: // Урон
        if (damageUpgradeLevel < 6) {
            attackDamage *= 1.2f;
            damageUpgradeLevel++;
        }
        break;
    case 3: // Макс. здоровье
        if (maxHealthUpgradeLevel < 6) {
            maxHealth *= 1.2f;
            health = maxHealth; // Полное восстановление
            maxHealthUpgradeLevel++;
        }
        break;
    case 4: // Регенерация
        if (regenRateUpgradeLevel < 5) {
            regenRate += 0.2f;
            regenRateUpgradeLevel++;
        }
        break;
    default:
        break;
    }
}

bool Player::isAwaitingUpgrade() const {
    return awaitingUpgradeChoice;
}

void Player::setAwaitingUpgrade(bool state) {
    awaitingUpgradeChoice = state;
}

void Player::generateUpgradeChoices() {
    upgradeChoices.clear();
    std::vector<int> allChoices = { 0, 1, 2, 3, 4 };
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(allChoices.begin(), allChoices.end(), g);

    for (int i = 0; i < 3; ++i) {
        upgradeChoices.push_back(allChoices[i]);
    }

}

void Player::updateSkill(float deltaTime) {
    if (skillCooldownTimer > 0.0f) {
        skillCooldownTimer -= deltaTime;
        if (skillCooldownTimer < 0.0f) skillCooldownTimer = 0.0f;
    }

    if (skillActive) {
        skillTimer -= deltaTime;
        if (skillTimer <= 0.0f) {
            skillActive = false;
        }
    }

    GLFWwindow* window = glfwGetCurrentContext();
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (!skillActive && skillCooldownTimer <= 0.0f) {
            skillActive = true;
            skillTimer = skillDuration;
            skillCooldownTimer = skillCooldown;
            specialAbilityUses++;
        }
    }
}

bool Player::isSkillActive() const {
    return skillActive;
}

bool Player::isSkillReady() const {
    return skillCooldownTimer <= 0.0f;
}

float Player::getSkillCooldown() const {
    return skillCooldownTimer;
}