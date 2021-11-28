#pragma once

#include "MovingGameObject.hpp"
#include "CollisionSystem.hpp"
#include "Scene.hpp"

#include <sstream>

using namespace std;

class MovingBlockObject : public MovingGameObject {
public:
    MovingBlockObject();
    MovingBlockObject(float mass, vector<string> properties,
        const glm::vec3& pos = glm::vec3(0.f),
        float w = 1.f, float h = 1.f,
        const glm::vec3& vel = glm::vec3(0.f),
        bool isFixed = false,
        const std::string& filename = "",
        const std::string& box_name = "",
        float l = 100.f);
    ~MovingBlockObject();

private:

};