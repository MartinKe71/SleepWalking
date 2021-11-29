/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:53
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-29 01:30:20
 * @ Description: Interface of square object
 */

#ifndef SquareObject_h
#define SquareObject_h

#include "GameObject.hpp"

using namespace std;

class HealthBarObject : public GameObject {
public:
    HealthBarObject();
    HealthBarObject(float maxHealth, 
        const glm::vec3& pos = glm::vec3(0.f), float w = 0.5f,
        float h = 0.1f, 
        const std::string& filename = "");
    ~HealthBarObject();
    
    void setHealth(float w);
    void increase();
    void decrease();
    
    void reset() override;
    void createVerts() override;
    
private:
    float width;
    float height;
    float maxWidth;
    float step;
};
#endif /* SquareObject_h */
