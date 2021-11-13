#include "SecondPlayerObject.hpp"
#include "load_save_png.hpp"


SecondPlayerObject::SecondPlayerObject() {
}

SecondPlayerObject::SecondPlayerObject(float mass, const glm::vec3& pos, float w, float h,
    const glm::vec3& vel, bool isFixed, const std::string& filename, float l) :
    GameObject(mass, pos, vel, isFixed, filename, l), width(w), height(h) {

    PlayerStats::Instance().player2SavedPos = pos;
    PlayerStats::Instance().player2SavedVel = vel;

    if (!filename.empty())
        load_png(data_path(filename),
            &(sz), &(pic), OriginLocation::LowerLeftOrigin);
    else {
        pic = vector<glm::u8vec4>(1, glm::u8vec4(0xff));
        sz = glm::uvec2(1, 1);
    }

    createVerts();
    prepareDraw();

    /*box = std::shared_ptr<PlayerCollisionBox>(new PlayerCollisionBox(position, glm::vec2(width, height), false));
    CollisionSystem::Instance().player1_collision = box;*/
}

SecondPlayerObject::~SecondPlayerObject() {

}


void SecondPlayerObject::createVerts() {

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

void SecondPlayerObject::reset() {
    std::cout << "reset beng" << std::endl;
    model = glm::mat4(1.0f);
    GameObject::reset();

    PlayerStats::Instance().reset();

    position = PlayerStats::Instance().player2SavedPos;
    velocity = PlayerStats::Instance().player2SavedVel;

    std::cout << "Before beng" << std::endl;

    vertex_positions.clear();
    createVerts();
    prepareDraw();
}

void SecondPlayerObject::update(float elapsed) {
    std::cout << "update second player\n";
    if (left.pressed && !right.pressed) {
        // move left
        std::cout << "second player move left\n";
        glm::vec2 new_pos = glm::vec2{ position.x, position.y };
        new_pos += elapsed * speed * glm::vec2(-1.f, 0.f) * glm::mat2(PlayerStats::Instance().rotMat);
        position.x = new_pos.x;
        position.y = new_pos.y;
        cout << "checking collision\n";
    }
    else if (!left.pressed && right.pressed) {
        // move right
        glm::vec2 new_pos = glm::vec2{ position.x, position.y };
        new_pos += elapsed * speed * glm::vec2(1.f, 0.f) * glm::mat2(PlayerStats::Instance().rotMat);
        position.x = new_pos.x;
        position.y = new_pos.y;
    }

    if (up.pressed && !down.pressed) {
        // move left
        glm::vec2 new_pos = glm::vec2{ position.x, position.y };
        new_pos += elapsed * speed * glm::vec2(0.f, 1.f) * glm::mat2(PlayerStats::Instance().rotMat);
        cout << "checking collision\n";
        position.x = new_pos.x;
        position.y = new_pos.y;
    }
    else if (!up.pressed && down.pressed) {
        // move right
        glm::vec2 new_pos = glm::vec2{ position.x, position.y };
        new_pos += elapsed * speed * glm::vec2(0.f, -1.f) * glm::mat2(PlayerStats::Instance().rotMat);
        position.x = new_pos.x;
        position.y = new_pos.y;
    }

    if (!isFixed) {
        glm::vec3 accel = force / mass;
        cout << "Force: " << glm::to_string(force) << endl;
        velocity += accel * elapsed;
        glm::vec3 new_pos = position + velocity * elapsed;
        cout << "new_pos: " << glm::to_string(new_pos) << endl;
        cout << "\n";
        if (!CollisionSystem::Instance().PlayerCheckCollision(glm::vec2{ new_pos.x, new_pos.y }, glm::vec2{ width * 2, height * 2 })) {
            cout << "Graivity Not colliding\n";

            position.x = new_pos.x;
            position.y = new_pos.y;
        }
        else {
            cout << " Gravity collided\n";
            PlayerStats::Instance().canJump = true;
            velocity = glm::vec3(0.f);
        }
    }

    PlayerStats::Instance().player2Pos = position;

    CollisionSystem::Instance().PlayerCheckTrigger(glm::vec2{ position.x, position.y }, glm::vec2{ width, height });
    std::cout << "Triggers checked" << std::endl;

    //box->SetPos(glm::vec2{ position.x, position.y });

    //if (position.x <= 0.f || position.y <= 0.f || PlayerStats::Instance().health <= 0.f) {
    //    reset();
    //}

    // std::cout << "player position: x: " << transform->position.x << "; y: " << transform->position.y << "\n";

    if(anims.count(type) && anims[type]) 
        anims[type]->play(this->VAO, this->VBO_texcoords, sz, elapsed);
    
    type = "Idle";
}