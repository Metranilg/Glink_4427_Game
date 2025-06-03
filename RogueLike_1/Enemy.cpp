

#include "Enemy.h"
#include <cmath>
#include "stb_image.h"
#include <glad/glad.h>
#include <iostream>


Enemy::Enemy(float x, float y, const EnemyType& type)
    : x(x),
    y(y),
    size(type.size),
    speed(type.speed),
    health(type.health),
    damage(type.damage),
    type(type)  // важно сохранить тип врага, он используется в draw
{
    // Загрузка текстуры
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(type.texturePath.c_str(), &width, &height, &nrChannels, 4);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        GLenum internalFormat = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    }
    else {
        std::cerr << "Failed to load enemy texture: " << type.texturePath << std::endl;
    }
    stbi_image_free(data);
}

void Enemy::update(float playerX, float playerY, float deltaTime) {
    float dx = playerX - x;
    float dy = playerY - y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist > 0.0f) {
        dx /= dist;
        dy /= dist;
        x += dx * speed;
        y += dy * speed;

        isFacingRight = dx >= 0.0f;
    }

    animationTimer += deltaTime;
    while (animationTimer >= type.frameDuration) {
        animationTimer -= type.frameDuration;
        currentFrame = (currentFrame + 1) % type.frameCount;
    }
}

void Enemy::draw() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    float texWidth = (float)type.spriteWidth * type.frameCount;
    float frameUSize = (float)type.spriteWidth / texWidth;
    float u0 = currentFrame * frameUSize;
    float u1 = u0 + frameUSize;

    float halfSize = size;

    glPushMatrix();
    glTranslatef(x, y, 0.0f);

    if (isFacingRight) {
        glScalef(-1.0f, 1.0f, 1.0f);
    }

    glBegin(GL_QUADS);
    glTexCoord2f(u0, 0.0f); glVertex2f(-halfSize, -halfSize);
    glTexCoord2f(u1, 0.0f); glVertex2f(halfSize, -halfSize);
    glTexCoord2f(u1, 1.0f); glVertex2f(halfSize, halfSize);
    glTexCoord2f(u0, 1.0f); glVertex2f(-halfSize, halfSize);
    glEnd();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}
