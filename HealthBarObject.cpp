/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:37
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-29 17:29:32
 * @ Description: implementation of square object
 */

#include "HealthBarObject.hpp"
#include "Inivar.hpp"
#include "PlayerStats.hpp"

HealthBarObject::HealthBarObject(){
}

HealthBarObject::HealthBarObject(float maxHealth, const glm::vec3& pos, 
    float w, float h, const std::string& filename): 
    UIObject(10.f, glm::vec3(pos.x - w/2.f, pos.y, pos.z), filename), 
    width(w), height(h), maxWidth(w){
    color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    step = w / maxHealth;
    
    createVerts();
    prepareDraw();
}

HealthBarObject::~HealthBarObject(){}

void HealthBarObject::createVerts(){

    vertex_positions = vector<glm::vec4>({
        glm::vec4(width,  height/2, 0.0f, 1.0f),  // top right
        glm::vec4(width, -height/2, 0.0f, 1.0f),  // bottom right
        glm::vec4(0.f, -height/2, 0.0f, 1.0f),  // bottom left
        glm::vec4(0.f,  height/2, 0.0f, 1.0f)  // top left 
    });

    vertex_texcoords = vector<glm::vec2>({
        glm::vec2(1.f,1.f),
        glm::vec2(1.f,0.f),
        glm::vec2(0.f,0.f),
        glm::vec2(0.f,1.f)
    });

    indices = std::vector<unsigned int>({ 0, 1, 2, 2, 3, 0 });
}

void HealthBarObject::reset(){
    GameObject::reset();
    vertex_positions.clear();
}

void HealthBarObject::setHealth(float w){
    width = w * step;
    
    vertex_positions.clear();
    
    createVerts();
    prepareDraw();
}

void HealthBarObject::increase(){
    if (width + step < maxWidth)
        width += step;
    else
        width = maxWidth;

    vertex_positions.clear();
    createVerts();
    prepareDraw();

}

void HealthBarObject::decrease(){
    if (width - step > 0.f)
        width -= step;
    else
        width = 0.f;
    
    vertex_positions.clear();
    
    createVerts();
    prepareDraw();
}