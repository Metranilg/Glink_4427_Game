// Enemy.h
#pragma once
#include <string>
#include <glad/glad.h>

struct EnemyType {
    std::string texturePath;
    float       health;
    float       damage;
    float       size;
    float       speed;
    int         frameCount;
    float       frameDuration;
    int         spriteWidth;
    int         spriteHeight;
};

class Enemy {
public:
    float x, y;
    float size;
    float speed;
    float health;
    float damage;

    bool isFacingRight = true;

    void takeDamage(float amount);      // ����� �����, ������� ���������� ����
    void drawDamageText();              // ������ ������� �����

    unsigned int textureID = 0;

    EnemyType type;       // ������ ���� �����

    int currentFrame = 0;
    float animationTimer = 0.0f;

    // ����������� ������ � ����� �����������:
    Enemy(float x, float y, const EnemyType& type);

    void update(float playerX, float playerY, float deltaTime);
    void draw();
};
