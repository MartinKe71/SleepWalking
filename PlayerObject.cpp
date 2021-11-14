#include "PlayerObject.hpp"
#include "load_save_png.hpp"
#include "AudioSystem.hpp"


PlayerObject::PlayerObject() {
}

PlayerObject::PlayerObject(float mass, const glm::vec3& pos, float w, float h,
    const glm::vec3& vel, bool isFixed, const std::string& filename, float l) :
    GameObject(mass, pos, vel, isFixed, filename, l), width(w), height(h) {

    PlayerStats::Instance().player1SavedPos = pos;
    PlayerStats::Instance().player1SavedVel = vel;

    createVerts();
    prepareDraw();

    box = std::shared_ptr<PlayerCollisionBox>(new PlayerCollisionBox(position, glm::vec2(width, height), false));
    CollisionSystem::Instance().player1_collision = box;
}

PlayerObject::~PlayerObject() {

}


void PlayerObject::createVerts() {

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

void PlayerObject::reset() {
    std::cout << "reset beng" << std::endl;
    model = glm::mat4(1.0f);
    GameObject::reset();

    PlayerStats::Instance().reset();

    position = PlayerStats::Instance().player1SavedPos;
    velocity = PlayerStats::Instance().player1SavedVel;
    box->SetPos(glm::vec2{ position.x, position.y });

    std::cout << "Before beng" << std::endl;

    vertex_positions.clear();
    createVerts();
    prepareDraw();
}

void PlayerObject::update(float elapsed) {
    std::cout << "update player\n";
    if (left.pressed && !right.pressed) {
        type = "Run";
        // move left
        glm::vec2 new_pos = glm::vec2{ position.x, position.y };
        new_pos += elapsed * speed * glm::vec2(-1.f, 0.f) * glm::mat2(PlayerStats::Instance().rotMat);
        PlayerStats::Instance().isFacingLeft = false;
        cout << "checking collision\n";
        if (!CollisionSystem::Instance().PlayerCheckCollision(new_pos, 
            glm::abs(glm::vec2{ width * 2, height * 2} * glm::mat2(PlayerStats::Instance().rotMat)))) {
            cout << "Not colliding\n";
            position.x = new_pos.x;
            position.y = new_pos.y;
        }
        if (PlayerStats::Instance().canJump) AudioSystem::Instance().PlayLongAudio(AudioSourceList::Footsteps, 3.0f);
    }
    else if (!left.pressed && right.pressed) {
        type = "Run";
        // move right
        glm::vec2 new_pos = glm::vec2{ position.x, position.y };
        new_pos += elapsed * speed * glm::vec2(1.f, 0.f) * glm::mat2(PlayerStats::Instance().rotMat);
        PlayerStats::Instance().isFacingLeft = true;
        if (!CollisionSystem::Instance().PlayerCheckCollision(new_pos, 
            glm::abs(glm::vec2{width * 2, height * 2} * glm::mat2(PlayerStats::Instance().rotMat)))) {
            position.x = new_pos.x;
            position.y = new_pos.y;
        }
        if (PlayerStats::Instance().canJump) AudioSystem::Instance().PlayLongAudio(AudioSourceList::Footsteps, 3.0f);
    }
    else if (!left.pressed && !right.pressed) {
        AudioSystem::Instance().StopLongAudio(AudioSourceList::Footsteps);
    }

    if (!PlayerStats::Instance().canJump) PlayerStats::Instance().jumpElapsed += elapsed;

    if (space.pressed && PlayerStats::Instance().canJump) {
        cout << "jump\n";
        type = "Jump";
        force += mass * jump_power * glm::vec3(0.f, 1.f, 0.f) * PlayerStats::Instance().rotMat;
        AudioSystem::Instance().PlayShortAudio(AudioSourceList::Jump);
        AudioSystem::Instance().StopLongAudio(AudioSourceList::Footsteps);
        space.pressed = false;
        PlayerStats::Instance().canJump = false;
    }

    if (!isFixed) {
        glm::vec3 accel = force / mass;
        cout << "Force: " << glm::to_string(force) << endl;
        velocity += accel * elapsed;
        glm::vec3 new_pos = position + velocity * elapsed;
        cout << "new_pos: " << glm::to_string(new_pos) << endl;
        cout << "\n";
        if (!CollisionSystem::Instance().PlayerCheckCollision(glm::vec2{ new_pos.x, new_pos.y }, 
            glm::abs(glm::vec2{ width * 2, height * 2} * glm::mat2(PlayerStats::Instance().rotMat)))) {
            cout << "Graivity Not colliding\n";
            
            position.x = new_pos.x;
            position.y = new_pos.y;
        }
        else {
            cout << " Gravity collided\n";
            if (PlayerStats::Instance().jumpElapsed > 0.5f) {
                PlayerStats::Instance().canJump = true;
                PlayerStats::Instance().jumpElapsed = 0.f;
            }
            velocity = glm::vec3(0.f);
        }
    }

    PlayerStats::Instance().player1Pos = position;

    CollisionSystem::Instance().PlayerCheckTrigger(glm::vec2{ position.x, position.y }, glm::vec2{ width, height });
    std::cout << "Triggers checked" << std::endl;

    CollisionSystem::Instance().PlayerCheckCollectables(glm::vec2{ position.x, position.y }, glm::vec2{ width, height });


    box->SetPos(glm::vec2{ position.x, position.y });

    //if (position.x <= 0.f || position.y <= 0.f || PlayerStats::Instance().health <= 0.f) {
    //    reset();
    //}

    // std::cout << "player position: x: " << transform->position.x << "; y: " << transform->position.y << "\n";

    // update texcoords every update
    if(anims.count(type) && anims[type]) 
        anims[type]->play(this->VAO, this->VBO_texcoords, sz, elapsed);
    
    type = "Idle";
}