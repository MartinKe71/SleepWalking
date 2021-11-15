#include "MovingThornObject.hpp"
#include "load_save_png.hpp"
#include "AudioSystem.hpp"


MovingThornObject::MovingThornObject() {
}

MovingThornObject::MovingThornObject(float mass, vector<string> properties, const glm::vec3& pos, float w, float h,
    const glm::vec3& vel, bool isFixed, const std::string& filename, const std::string& box_name,
    float l) :MovingGameObject::MovingGameObject(mass, properties, pos, w, h, vel, isFixed, filename, box_name, l) {
    
    box = CollisionSystem::Instance().AddOneThornBlock(glm::vec2(pos.x, pos.y),
        glm::vec2(width * 2, height * 2),
        box_name);

    //box = CollisionSystem::Instance().AddOneSceneBlock(glm::vec2(position.x, position.y),
    //    glm::vec2(width * 2, height * 2),
    //    box_name);
    box->owner = this;
}

MovingThornObject::~MovingThornObject() {

}