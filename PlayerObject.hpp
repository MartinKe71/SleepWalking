#include "GameObject.hpp"
#include "CollisionSystem.hpp"
#include "Scene.hpp"

using namespace std;

class PlayerObject : public GameObject {
public :
	PlayerObject();
    PlayerObject(float mass, const glm::vec3& pos = glm::vec3(0.f),
        float w = 1.f, float h = 1.f,
        const glm::vec3& vel = glm::vec3(0.f),
        bool isFixed = false,
        const std::string& filename = "", float l = 100.f);
    ~PlayerObject();

    std::shared_ptr<PlayerCollisionBox> box;

    //input tracking:
    struct Button {
        uint8_t downs = 0;
        bool pressed = 0;
    } left, right, space;

    int direction = 1;
    bool canJump = true;

    virtual void update(float elapsed) override;
    virtual void reset() override;
    virtual void createVerts() override;

private:
    float width, height;
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};