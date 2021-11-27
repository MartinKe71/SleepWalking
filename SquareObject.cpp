/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:37
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-26 01:58:01
 * @ Description: implementation of square object
 */

#include "load_save_png.hpp"
#include "data_path.hpp"
#include "SquareObject.hpp"
#include "Inivar.hpp"
#include "PlayerStats.hpp"

SquareObject::SquareObject(){
}

SquareObject::SquareObject(float mass, const glm::vec3& pos, 
    const glm::vec3& vel, bool isFixed, float r, 
    const std::string& filename, float l): 
    GameObject(mass, pos, vel, isFixed, filename, l), width(r){
    
    createVerts();
    prepareDraw();
}

SquareObject::~SquareObject(){}

void SquareObject::createVerts(){

    vertex_positions = vector<glm::vec4>({
        glm::vec4(width/2,  width/2, 0.0f, 1.0f),  // top right
        glm::vec4(width/2, -width/2, 0.0f, 1.0f),  // bottom right
        glm::vec4(-width/2, -width/2, 0.0f, 1.0f),  // bottom left
        glm::vec4(-width/2,  width/2, 0.0f, 1.0f)  // top left 
    });

    vertex_texcoords = vector<glm::vec2>({
        glm::vec2(1.f,1.f),
        glm::vec2(1.f,0.f),
        glm::vec2(0.f,0.f),
        glm::vec2(0.f,1.f)
    });

    indices = std::vector<unsigned int>({ 0, 1, 2, 2, 3, 0 });
}

void SquareObject::reset(){
    GameObject::reset();
    vertex_positions.clear();
}

void SquareObject::setWidth(float r){
    width = r;
    
    vertex_positions.clear();
    
    createVerts();
    prepareDraw();
}

void SquareObject::fadeOut() {
    if (color.w > 0.f)
        color.w -= MARKER_FADE_OUT;

    // // done fading out
    // if (color.w <= 0.f){
    //     color.w = 0.f;
    //     return true;
    // }
    // // still fading out
    // return false;
}

void SquareObject::show(const glm::vec3& pos) {
    position = pos + glm::vec3(0.f, 7.f, 0.f) * PlayerStats::Instance().rotMat;
    color.w = 1.f;
}