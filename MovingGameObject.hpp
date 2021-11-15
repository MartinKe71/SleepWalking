#pragma once

#include "GameObject.hpp"
#include "CollisionSystem.hpp"
#include "Scene.hpp"

#include <sstream>

using namespace std;

class MovingGameObject : public GameObject {
public:
    MovingGameObject();
    MovingGameObject(float mass, vector<string> properties, 
        const glm::vec3& pos = glm::vec3(0.f),
        float w = 1.f, float h = 1.f,
        const glm::vec3& vel = glm::vec3(0.f),
        bool isFixed = false,
        const std::string& filename = "", 
        const std::string& box_name = "", 
         float l = 100.f);
    ~MovingGameObject();

    std::shared_ptr<CollisionBox> box;

    float getMovingSpeed() { return moving_speed; }
    float getMovingRange() { return moving_range; }

    virtual void update(float elapsed) override;
    virtual void reset() override;
    virtual void createVerts() override;

protected:
    float width, height;
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    glm::vec3 start_position;

    glm::vec3 moving_dir = glm::vec3(1.f, 0.f, 0.f);

    float moving_speed = 20.f;
    float moving_range = 30.f;    
};