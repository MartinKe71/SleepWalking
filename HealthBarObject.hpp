/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:53
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-29 17:30:18
 * @ Description: Interface of square object
 */

#ifndef HealthBarObject_h
#define HealthBarObject_h

#include "UIObject.hpp"

using namespace std;

class HealthBarObject : public UIObject {
public:
    HealthBarObject();
    HealthBarObject(float maxHealth, 
        const glm::vec3& pos = glm::vec3(0.f), float w = 0.7f,
        float h = 0.05f, 
        const std::string& filename = "");
    ~HealthBarObject();
    
    void setHealth(float w);
    void increase();
    void decrease();

    void setPos(const glm::vec3& p){
        position = glm::vec3(p.x - maxWidth / 2.f, p.y, p.z);
    }
    
    void reset() override;
    void createVerts() override;
    
private:
    float width;
    float height;
    float maxWidth;
    float step;
};
#endif /* HealthBarObject_h */
