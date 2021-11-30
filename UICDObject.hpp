/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:53
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-29 21:34:38
 * @ Description: Interface of square object
 */

#ifndef UICDObject_h
#define UICDObject_h

#include "UIObject.hpp"

using namespace std;

class UICDObject : public UIObject {
public:
    UICDObject();
    UICDObject(const glm::vec3& pos, float r, float cd,
        const std::string& filename = "");
    ~UICDObject();
    
    void setMaxCD(float cd);
    void setCD(float cd);
    void show();
    void hide();
    
    void reset() override;
    void createVerts() override;
    
private:
    float radius;
    int numVert;
    float angle;
    float maxCD;
};
#endif /* UICDObject_h */
