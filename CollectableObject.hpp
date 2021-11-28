#pragma once

#include "GameObject.hpp"
#include "CollisionBox.hpp"

using namespace std;

class CollectableObject : public GameObject {
public:
    CollectableObject();
    CollectableObject(float mass, const glm::vec3& pos = glm::vec3(0.f),
        float w = 1.f, float h = 1.f,
        const glm::vec3& vel = glm::vec3(0.f),
        bool isFixed = false,
        const std::string& filename = "", float l = 100.f);
    ~CollectableObject();

    void reset() override;
    void createVerts() override;

    std::shared_ptr<CollectableCollisionBox> box;

private:
    float width, height;
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};