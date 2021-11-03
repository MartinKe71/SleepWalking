/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:53
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-02 21:59:51
 * @ Description: Interface of square object
 */

#ifndef SquareObject_h
#define SquareObject_h

#include "GameObject.hpp"

using namespace std;

class SquareObject : public GameObject {
public:
    SquareObject();
    // SquareObject(float m, float r);
    // SquareObject(float mass, loat r);
    SquareObject(float mass, const glm::vec3& pos = glm::vec3(0.f), 
        const glm::vec3& vel = glm::vec3(0.f), 
        bool isFixed = false, float r = 0.1f,
        const std::string& filename = "", float l = 100.f);
    ~SquareObject();
    
    glm::vec3 computeNormal();
    
    void setWidth(float r);
    
    void draw(Scene::Camera const &camera);
    void reset();
    
    void prepareDraw();
    void createVerts();
    void createIndices();
    
    //glm::vec3 getNormal() { return normal; }
    
private:
    float width;
    
    GLuint VAO;
    GLuint VBO_positions, VBO_texcoords, EBO;
    GLuint tex;
    
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vector<glm::vec4> vertex_positions;
    vector<glm::vec4> vertex_normals;
    vector<glm::vec2> vertex_texcoords;
    vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

    // texture info
    glm::uvec2 sz;
    std::vector< glm::u8vec4 > pic;
};
#endif /* SquareObject_h */
