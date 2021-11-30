/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:53
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-29 18:11:48
 * @ Description: Interface of square object
 */

#ifndef UIBGObject_h
#define UIBGObject_h

#include "UIObject.hpp"

using namespace std;

class UIBGObject : public UIObject {
public:
    UIBGObject();
    UIBGObject(const glm::vec3& pos, float w, float h, 
        const std::string& filename = "");
    ~UIBGObject();
    
    void reset() override;
    void createVerts() override;
    
private:
    float width;
    float height;
};
#endif /* UIBGObject_h */
