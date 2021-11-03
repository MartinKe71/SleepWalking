/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:37
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-03 00:45:21
 * @ Description: implementation of square object
 */

#include "load_save_png.hpp"
#include "data_path.hpp"
#include "SquareObject.hpp"
#include "Inivar.hpp"

SquareObject::SquareObject(){
}

SquareObject::SquareObject(float mass, const glm::vec3& pos, 
    const glm::vec3& vel, bool isFixed, float r, 
    const std::string& filename, float l): 
    GameObject(mass, pos, vel, isFixed, l), width(r){
    
    if (!filename.empty())
        load_png(data_path(filename), 
            &(sz), &(pic), OriginLocation::LowerLeftOrigin);
    else {
        pic = vector<glm::u8vec4> (1, glm::u8vec4(0xff));
        sz = glm::uvec2(1, 1);
    }
        
    
    createVerts();
    
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO_positions));
    GLCall(glGenBuffers(1, &VBO_texcoords));
    GLCall(glGenBuffers(1, &EBO));
    GLCall(glGenTextures(1, &tex));

    prepareDraw();
}

SquareObject::~SquareObject(){
    // Delete the VBOs and the VAO.
   glDeleteBuffers(1, &VBO_positions);
   glDeleteBuffers(1, &EBO);
   glDeleteVertexArrays(1, &VAO);
}

void SquareObject::draw(Scene::Camera const &camera){
    glm::mat4 world_to_clip = camera.make_projection() * glm::mat4(camera.transform->make_world_to_local());

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
    //    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, tex));

    // draw the points using triangles, indexed with the EBO
    GLCall(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0));

    // Unbind the VAO and shader program
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
}


void SquareObject::prepareDraw(){
    
    // Bind to the VAO.
    GLCall(glBindVertexArray(VAO));

    // Bind to the first VBO - We will use it to store the vertices
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_positions));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * vertex_positions.size(), vertex_positions.data(), GL_STATIC_DRAW));
    GLCall(glEnableVertexAttribArray(shader->Position_vec4));
    GLCall(glVertexAttribPointer(shader->Position_vec4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0));

    // Bind to the second VBO - We will use it to store the vertex_texcoords
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
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	
	GL_ERRORS(); //PARANOIA: make sure nothing strange happened during setup
}


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
