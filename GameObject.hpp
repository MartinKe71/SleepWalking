/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 17:59:51
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-29 12:47:48
 * @ Description: Header file for GameObject class
 */

#ifndef GameObject_h
#define GameObject_h

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

#include "ColorTextureProgram.hpp"
#include "ObjectColorTextureProgram.hpp"
#include "Scene.hpp"
#include "Animation2D.hpp"
#include "glm/gtx/euler_angles.hpp"

class GameObject {
public:
    GameObject();
    GameObject(float mass, const glm::vec3& pos = glm::vec3(0.f), 
        const glm::vec3& vel = glm::vec3(0.f), 
        bool isFixed = false, const std::string& filename = "", float l = 100.f);
    
    virtual ~GameObject();

    void applyForce(glm::vec3& f) {
        force += f;
    }
    
    float getMass() { return mass; }
    glm::vec3 getPos() { return position; }
    glm::vec3 getNormal() { return normal; }
    glm::vec3 getVelocity() { return velocity; }
    glm::vec3 getForce() { return force; }
    float getLife(){return lifeSpan;}

    
    void zeroNormal(){
        normal = glm::vec3(0.0f);
    }
    void zeroForce(){
        force = glm::vec3(0.0f);
    }
    
    void setIsFixed(bool option){
        isFixed = option;
    }
    
    void setLife(float life){
        lifeSpan = (life <= 0.0f) ? 0.0f : life;
    }
    
    void setPos(const glm::vec3& p){
        position = p;
    }
    
    void setVelocity(const glm::vec3& v){
        velocity = v;
    }

    void applyNormal(glm::vec3& n) {
        normal += n;
    }
    
    void normalizeNormal(){
        normal = glm::normalize(normal);
    }

    void applyRotation(const glm::vec3& euler, const glm::vec3& camPos){
        model = model * glm::eulerAngleYXZ(euler.y, euler.x, euler.z);
    }
    
    void addAnimation(const std::string& type, const std::string& filename);

    virtual void createVerts() = 0;

    virtual void update (float deltaTime);
    virtual void draw (Scene::Camera const &camera);
    virtual void prepareDraw();
    virtual void reset();
    
protected:
    float mass; // constant
    glm::vec3 position; // evaluate frame by frame
    glm::vec3 velocity; // evaluate frame by frame
    glm::vec3 force;    // reset and recomput each frame
    glm::vec3 normal;
    bool isFixed;
    float lifeSpan;

    glm::mat4 model;
    
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // shader info
    //Load< ColorTextureProgram > shader = color_texture_program;
    Load< ObjectColorTextureProgram > shader = object_color_texture_program;

    GLuint VAO;
    GLuint VBO_positions, VBO_texcoords, EBO;
    GLuint tex;

    std::vector<glm::vec4> vertex_positions;
    std::vector<glm::vec2> vertex_texcoords;
    std::vector<unsigned int> indices;

    // texture info
    glm::uvec2 sz;
    std::vector< glm::u8vec4 > pic;

    // animation info
    std::unordered_map<std::string, Animation2D*> anims;
    std::string type = "Idle";
};

#endif /* GameObject_h */
