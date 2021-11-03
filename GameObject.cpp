/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 17:59:16
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-02 23:38:39
 * @ Description: Base class for game object
 */

#include "GameObject.hpp"

GameObject::GameObject() : mass (0), position(glm::vec3(0.0f)), 
    velocity(glm::vec3(0.0f)), force(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), 
    isFixed(false), model(glm::mat4(1.0f)){
    lifeSpan = 100.0f;
}

GameObject::GameObject(float mass, const glm::vec3& pos, 
    const glm::vec3& vel, bool isFixed, float l) : mass (mass), position(pos), 
    velocity(vel), force(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), 
    isFixed(isFixed), lifeSpan(l), model(glm::mat4(1.0f)){
 
}

GameObject::~GameObject(){}

void GameObject::update(float deltaTime){
    // compute acceleration
    if (!isFixed && lifeSpan > 0.0f){
        glm::vec3 accel = force / mass;
        velocity += accel * deltaTime;
        position += velocity * deltaTime;
    }
    
}

void GameObject::reset(){
    position = glm::vec3(0.0f);
    velocity = glm::vec3(0.0f);
    zeroForce();
    zeroNormal();
    isFixed = false;
    lifeSpan = 0.0f;
}
