#include "MovingGameObject.hpp"
#include "load_save_png.hpp"
#include "AudioSystem.hpp"


MovingGameObject::MovingGameObject() {
}

MovingGameObject::MovingGameObject(float mass, vector<string> properties, const glm::vec3& pos, float w, float h,
    const glm::vec3& vel, bool isFixed, const std::string& filename, const std::string& box_name, 
    float l) :
    GameObject(mass, pos, vel, isFixed, filename, l), width(w), height(h) {

    start_position = pos;

    if (properties[2] == "H") moving_dir = glm::vec3(1.f, 0.f, 0.f);
    else moving_dir = glm::vec3(0.f, 1.f, 0.f);

    moving_speed = std::stof(properties[3]);
    moving_range = std::stof(properties[4]);

    createVerts();
    prepareDraw();

    //box = CollisionSystem::Instance().AddOneThornBlock(glm::vec2(position.x, position.y),
    //    glm::vec2(width * 2, height * 2),
    //    box_name);

    ////box = CollisionSystem::Instance().AddOneSceneBlock(glm::vec2(position.x, position.y),
    ////    glm::vec2(width * 2, height * 2),
    ////    box_name);
    //box->owner = this;
}

MovingGameObject::~MovingGameObject() {

}


void MovingGameObject::createVerts() {

    vertex_positions = vector<glm::vec4>({
        glm::vec4(width,  height, 0.0f, 1.0f),  // top right
        glm::vec4(width, -height, 0.0f, 1.0f),  // bottom right
        glm::vec4(-width, -height, 0.0f, 1.0f),  // bottom left
        glm::vec4(-width,  height, 0.0f, 1.0f)  // top left 
        });

    vertex_texcoords = vector<glm::vec2>({
        glm::vec2(1.f,1.f),
        glm::vec2(1.f,0.f),
        glm::vec2(0.f,0.f),
        glm::vec2(0.f,1.f)
        });

    indices = std::vector<unsigned int>({ 0, 1, 2, 2, 3, 0 });
}

void MovingGameObject::reset() {

}

void MovingGameObject::update(float elapsed) {
    cout << "updating moving object" << endl;

    position += moving_dir * moving_speed * elapsed;

    if (glm::length(position - start_position) > moving_range) {
        moving_dir *= -1.f;
        position += 2.f * moving_dir * (glm::length(position - start_position) - moving_range);
    }

    if (box != nullptr) {
        cout << "box is not nullptr\n\n" << endl;
        box->SetPos(position);
    }
}