/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 17:59:16
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-12 17:17:22
 * @ Description: Base class for game object
 */

#include "GameObject.hpp"
#include "Inivar.hpp"
#include "GLCall.hpp"
#include "load_save_png.hpp"

#include "glm/gtx/string_cast.hpp"

GameObject::GameObject() : mass (0), position(glm::vec3(0.0f)), 
    velocity(glm::vec3(0.0f)), force(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), 
    isFixed(false), model(glm::mat4(1.0f)){
        
    lifeSpan = 100.0f;
    pic = std::vector<glm::u8vec4>(1, glm::u8vec4(0xff));
    sz = glm::uvec2(1, 1);

    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO_positions));
    GLCall(glGenBuffers(1, &VBO_texcoords));
    GLCall(glGenBuffers(1, &EBO));
    GLCall(glGenTextures(1, &tex));
}

GameObject::GameObject(float mass, const glm::vec3& pos, 
    const glm::vec3& vel, bool isFixed, const std::string& filename, float l) : mass (mass), position(pos), 
    velocity(vel), force(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), 
    isFixed(isFixed), lifeSpan(l), model(glm::mat4(1.0f)){

    if (!filename.empty())
        load_png(data_path(filename),
            &(sz), &(pic), OriginLocation::LowerLeftOrigin);
    else {
        pic = std::vector<glm::u8vec4>(1, glm::u8vec4(0xff));
        sz = glm::uvec2(1, 1);
    }

    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO_positions));
    GLCall(glGenBuffers(1, &VBO_texcoords));
    GLCall(glGenBuffers(1, &EBO));
    GLCall(glGenTextures(1, &tex));
}

GameObject::~GameObject(){
    // Delete the VBOs and the VAO.
    glDeleteBuffers(1, &VBO_positions);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    // for(auto& o : anims) delete o;
}

void GameObject::update(float deltaTime){
    // update texcoords every update
    if(anims.count(type) && anims[type]) 
        anims[type]->play(this->VAO, this->VBO_texcoords, sz, deltaTime);
    
    // compute acceleration
    if (!isFixed && lifeSpan > 0.0f){
        glm::vec3 accel = force / mass;
        velocity += accel * deltaTime;
        position += velocity * deltaTime;
    }
    
}

void GameObject::reset(){
    position = glm::vec3(0.0f);
    velocity = glm::vec3(0.0f);
    zeroForce();
    zeroNormal();
    //isFixed = false;
    lifeSpan = 0.0f;
}

void GameObject::prepareDraw() {

    // Bind to the VAO.
    GLCall(glBindVertexArray(VAO));

    // Bind to the first VBO - We will use it to store the vertices
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_positions));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * vertex_positions.size(), vertex_positions.data(), GL_STATIC_DRAW));
    GLCall(glEnableVertexAttribArray(shader->Position_vec4));
    GLCall(glVertexAttribPointer(shader->Position_vec4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0));
    
    // load texcoord every draw to create animation
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_texcoords));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertex_texcoords.size(), vertex_texcoords.data(), GL_STATIC_DRAW));
    GLCall(glEnableVertexAttribArray(shader->TexCoord_vec2));
    GLCall(glVertexAttribPointer(shader->TexCoord_vec2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0));

    // Generate EBO, bind the EBO to the bound VAO and send the data
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW));

    // Unbind the VBOs.
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindVertexArray(0));
    
    GLCall(glBindTexture(GL_TEXTURE_2D, tex));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sz.x, sz.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pic.data()));
    GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    GL_ERRORS(); //PARANOIA: make sure nothing strange happened during setup
}

void GameObject::draw(Scene::Camera const& camera) {
    

    glm::mat4 world_to_clip = camera.make_projection() * glm::mat4(camera.transform->make_world_to_local());
    glm::mat4x3 world_to_light = glm::mat4x3(1.0f) * glm::translate(glm::mat4(1.0f), position);
    glm::mat3 normal_to_light = glm::inverse(glm::transpose(glm::mat3(world_to_light)));

    // apply local rotation
    glm::mat4 curModel = glm::translate(glm::mat4(1.f), position) * model;
    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);
    //cout << glm::to_string(position) << endl;

    // actiavte the shader program
    GLCall(glUseProgram(shader->program));

    //cout << "\n\n----------------" << endl;
    //cout << "normal to light: " << glm::to_string(normal_to_light) << endl;
    //cout << "world to light: " << glm::to_string(world_to_light) << endl;

    //cout << "-------------------\n\n" << endl;

    // get the locations and send the uniforms to the shader
    GLCall(glUniformMatrix4fv(shader->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(world_to_clip)));
    GLCall(glUniformMatrix4x3fv(shader->OBJECT_TO_LIGHT_mat4x3, 1, GL_FALSE, glm::value_ptr(world_to_light)));
    GLCall(glUniformMatrix3fv(shader->NORMAL_TO_LIGHT_mat3, 1, GL_FALSE, glm::value_ptr(normal_to_light)));
    GLCall(glUniformMatrix4fv(shader->Model_mat4, 1, GL_FALSE, glm::value_ptr(curModel)));
    GLCall(glUniform4fv(shader->Color_vec4, 1, &color[0]));
    GLCall(glUniform3fv(shader->Normal_vec3, 1, glm::value_ptr(normal)));
    
    // Bind the VAO
    GLCall(glBindVertexArray(VAO));
    //    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, tex));

    // draw the points using triangles, indexed with the EBO
    GLCall(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0));

    // Unbind the VAO and shader program
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
}

void GameObject::addAnimation(const std::string& type, const std::string& filename){
    Animation2D* anim = new Animation2D(data_path(filename));
	anims[type] = anim;
}
