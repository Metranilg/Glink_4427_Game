// Entity.cpp
#include "Entity.h"

Entity::Entity(glm::vec2 pos, glm::vec2 sz)
    : position(pos), size(sz) {}

Entity::~Entity() {}

glm::vec2 Entity::getCenter() const {
    return position + size / 2.0f;
}
