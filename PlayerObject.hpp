#include "GameObject.hpp"
#include "CollisionSystem.hpp"

using namespace std;

class PlayerObject : public GameObject {
public :
	PlayerObject();
    PlayerObject(float mass, const glm::vec3& pos = glm::vec3(0.f),
        glm::uvec2 size = glm::uvec2(80, 100),
        const glm::vec3& vel = glm::vec3(0.f),
        bool isFixed = false,
        const std::string& filename = "", float l = 100.f);
    ~PlayerObject();

    CollisionBox* box;

    //input tracking:
    struct Button {
        uint8_t downs = 0;
        bool pressed = 0;
    } left, right, space;

    virtual void update(float elapsed) override;
    void draw(glm::uvec2 const& drawable_size);
    void reset() override;

    void prepareDraw();
    void createVerts();

private:
    glm::uvec2 size;

    GLuint VAO;
    GLuint VBO_positions, VBO_texcoords, EBO;

    glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    vector<glm::vec4> vertex_positions;
    vector<glm::vec4> vertex_normals;
    vector<glm::vec2> vertex_texcoords;
    vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

    // texture info
    glm::uvec2 sz;
    std::vector< glm::u8vec4 > pic;
};