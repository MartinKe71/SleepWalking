/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:53
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-29 16:18:40
 * @ Description: Interface of square object
 */

#ifndef UIObject_h
#define UIObject_h

#include "ColorTextureProgram.hpp"
#include "GameObject.hpp"

using namespace std;

class UIObject : public GameObject {
public:
    UIObject();
    UIObject(float mass, const glm::vec3& pos = glm::vec3(0.f),
        const std::string& filename = "");
    ~UIObject();

    virtual void prepareDraw() override;
    virtual void draw (Scene::Camera const &camera) override;
    
private:
    Load< ColorTextureProgram > shader = color_texture_program;
};
#endif /* UIObject_h */
