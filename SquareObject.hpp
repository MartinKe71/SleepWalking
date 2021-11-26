/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:53
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-26 01:50:11
 * @ Description: Interface of square object
 */

#ifndef SquareObject_h
#define SquareObject_h

#include "GameObject.hpp"

using namespace std;

class SquareObject : public GameObject {
public:
    SquareObject();
    SquareObject(float mass, const glm::vec3& pos = glm::vec3(0.f), 
        const glm::vec3& vel = glm::vec3(0.f), 
        bool isFixed = false, float r = 0.1f,
        const std::string& filename = "", float l = 100.f);
    ~SquareObject();
    
    void setWidth(float r);
    
    void reset() override;
    void createVerts() override;

    void fadeOut();
    void show(const glm::vec3& pos);
    
private:
    float width;
};
#endif /* SquareObject_h */
