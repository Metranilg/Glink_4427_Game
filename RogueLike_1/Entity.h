// Entity.h
#pragma once
#include <glm/glm.hpp>

class Entity {
public:
    glm::vec2 position;
    glm::vec2 size;

    Entity(glm::vec2 pos, glm::vec2 sz);
    virtual ~Entity();

    // можно добавить общие методы, например:
    glm::vec2 getCenter() const;
};
