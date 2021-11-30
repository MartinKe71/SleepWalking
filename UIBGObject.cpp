/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:37
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-29 17:47:10
 * @ Description: implementation of square object
 */

#include "UIBGObject.hpp"
#include "Inivar.hpp"
#include "PlayerStats.hpp"

UIBGObject::UIBGObject(){
}

UIBGObject::UIBGObject(const glm::vec3& pos, float w, float h, 
        const std::string& filename): 
    UIObject(10.f, pos, filename), 
    width(w), height(h){

    createVerts();
    prepareDraw();
}

UIBGObject::~UIBGObject(){}

void UIBGObject::createVerts(){

    vertex_positions = vector<glm::vec4>({
        glm::vec4(width/2.f,  height/2, 0.0f, 1.0f),  // top right
        glm::vec4(width/2.f, -height/2, 0.0f, 1.0f),  // bottom right
        glm::vec4(-width/2.f, -height/2, 0.0f, 1.0f),  // bottom left
        glm::vec4(-width/2.f,  height/2, 0.0f, 1.0f)  // top left 
    });

    vertex_texcoords = vector<glm::vec2>({
        glm::vec2(1.f,1.f),
        glm::vec2(1.f,0.f),
        glm::vec2(0.f,0.f),
        glm::vec2(0.f,1.f)
    });

    indices = std::vector<unsigned int>({ 0, 1, 2, 2, 3, 0 });
}

void UIBGObject::reset(){
    GameObject::reset();
    vertex_positions.clear();
}