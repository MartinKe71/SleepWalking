#include "load_save_png.hpp"
#include "data_path.hpp"
#include "CollectableObject.hpp"
#include "Inivar.hpp"

CollectableObject::CollectableObject() {
}

CollectableObject::CollectableObject(float mass, const glm::vec3& pos, float w, float h,
    const glm::vec3& vel, bool isFixed, const std::string& filename, float l) :
    GameObject(mass, pos, vel, isFixed, filename, l), width(w), height(h) {

    createVerts();
    prepareDraw();
}

CollectableObject::~CollectableObject() {
    std::cout << "deleting box\n";
    std::cout << "Use Count Prev: " << box.use_count() << std::endl;
    box.reset();
    std::cout << "Use Count After: " << box.use_count() << std::endl;

    std::cout << "deleting buffers\n";
    glDeleteBuffers(1, &VBO_positions);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    std::cout << "finished deletion\n";
}

void CollectableObject::createVerts() {

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

void CollectableObject::reset() {
    GameObject::reset();
    vertex_positions.clear();
}