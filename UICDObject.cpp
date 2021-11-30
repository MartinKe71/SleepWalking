/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:37
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-29 21:57:40
 * @ Description: implementation of square object
 */

#include "UICDObject.hpp"
#include "Inivar.hpp"
#include "PlayerStats.hpp"

UICDObject::UICDObject(){
}

UICDObject::UICDObject(const glm::vec3& pos, float r, float cd,
        const std::string& filename): 
    UIObject(10.f, pos, filename), 
    radius(r), numVert(100), angle(2.f*glm::pi<float>()), maxCD(cd){

    color = glm::vec4(0.f, 1.f, 0.f, 0.f);

    createVerts();
    prepareDraw();
}

UICDObject::~UICDObject(){}

void UICDObject::createVerts(){

    const float delta_angle = angle/numVert;

    vertex_texcoords.push_back(glm::vec2(0.5f, 0.5f));
    vertex_positions.push_back(glm::vec4(0.f, 0.f, 0.f, 1.f));

    for(int i = 0; i < numVert ; i++)
    {
        vertex_texcoords.push_back(glm::vec2(
            (glm::cos(delta_angle*i) + 1.f)*0.5f,
            (glm::sin(delta_angle*i) + 1.f)*0.5f));
        
        vertex_positions.push_back(glm::vec4(
            glm::cos(delta_angle*i) * radius,
            glm::sin(delta_angle*i) * radius,
            0.f,
            1.f
        ));
    }

    vertex_texcoords.push_back(glm::vec2(0.5f, 0.5f));
    vertex_positions.push_back(glm::vec4(0.f, 0.f, 0.f, 1.f));
    
    for(int i=1; i<=numVert; i++){
        int first = i;
        int second = first+ 1;
        indices.push_back(first);
        indices.push_back(second);
        indices.push_back(0);
    }
}

void UICDObject::setMaxCD(float cd){
    maxCD = cd;
}

void UICDObject::setCD(float cd){
    float frac = cd / maxCD;
    angle = 2.f * glm::pi<float>() - frac * 2.f * glm::pi<float>();
    
    vertex_positions.clear();
    createVerts();
    prepareDraw();
}

void UICDObject::hide() {
    color.w = 0.f;
}

void UICDObject::show() {
    angle = 2.f * glm::pi<float>();
    color.w = 1.f;
}

void UICDObject::reset(){
    GameObject::reset();
    vertex_positions.clear();
}