/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:37
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-29 21:15:03
 * @ Description: implementation of square object
 */

#include "GLCall.hpp"
#include "UIObject.hpp"
#include "Inivar.hpp"
#include "PlayerStats.hpp"

UIObject::UIObject(){
}

UIObject::UIObject(float mass, const glm::vec3& pos, const std::string& filename): 
    GameObject(mass, pos, glm::vec3(0.f), true, filename, 100.f){
}

UIObject::~UIObject(){}

void UIObject::prepareDraw() {

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

void UIObject::draw(Scene::Camera const& camera) {

    glm::mat4 world_to_clip = glm::mat4(
        1.0f / camera.aspect, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // apply local rotation
    glm::mat4 curModel = glm::translate(glm::mat4(1.f), position) * model;
    //cout << glm::to_string(position) << endl;

    // actiavte the shader program
    GLCall(glUseProgram(shader->program));

    // get the locations and send the uniforms to the shader
    GLCall(glUniformMatrix4fv(shader->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(world_to_clip)));
    GLCall(glUniformMatrix4fv(shader->Model_mat4, 1, GL_FALSE, (float*)&curModel));
    GLCall(glUniform4fv(shader->Color_vec4, 1, &color[0]));

    // Bind the VAO
    GLCall(glBindVertexArray(VAO));
    // GLCall(glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ));

    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, tex));

    // draw the points using triangles, indexed with the EBO
    GLCall(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0));

    // Unbind the VAO and shader program
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
}
