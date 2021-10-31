/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-10-30 18:35:37
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-10-31 03:48:16
 * @ Description: implementation of square object
 */

#include "load_save_png.hpp"
#include "data_path.hpp"
#include "SquareObject.hpp"
#include <signal.h>
#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

#ifdef _MSC_VER
#define DEBUG_BREAK __debugbreak()
#elif __APPLE__
#define DEBUG_BREAK __builtin_trap()
#else
#define DEBUG_BREAK raise(SIGTRAP)
#endif

#define ASSERT(x) if (!(x)) DEBUG_BREAK;
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] ( " << error << " ) : " << function << " " << file << ": " << line << std::endl;
		return false;
	}
	return true;
}

SquareObject::SquareObject(){
}

SquareObject::SquareObject(float mass, const glm::vec3& pos, 
    const glm::vec3& vel, bool isFixed, float r, 
    const std::string& filename, float l): 
    GameObject(mass, pos, vel, isFixed, l), width(r){
    
    glm::uvec2 sz (r, r);
    if (!filename.empty())
        load_png(data_path(filename), 
            &(sz), &(pic), OriginLocation::LowerLeftOrigin);
    
    createVerts();
    
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO_positions));
    GLCall(glGenBuffers(1, &VBO_normals));
    GLCall(glGenBuffers(1, &VBO_texcoords));
    GLCall(glGenBuffers(1, &EBO));

    prepareDraw();
}

SquareObject::~SquareObject(){
    // Delete the VBOs and the VAO.
   glDeleteBuffers(1, &VBO_positions);
   glDeleteBuffers(1, &VBO_normals);
   glDeleteBuffers(1, &EBO);
   glDeleteVertexArrays(1, &VAO);
}

void SquareObject::draw(glm::uvec2 const &drawable_size){
    float aspect = float(drawable_size.x) / float(drawable_size.y);
    glm::mat4 world_to_clip = glm::mat4(
        1.0f / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // actiavte the shader program

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    //cout << glm::to_string(position) << endl;
    
    // actiavte the shader program
    GLCall(glUseProgram(shader->program));

    // get the locations and send the uniforms to the shader
    GLCall(glUniformMatrix4fv(shader->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(world_to_clip)));
    GLCall(glUniformMatrix4fv(shader->Model_mat4, 1, GL_FALSE, (float*)&model));
    GLCall(glUniform4fv(shader->Color_vec4, 1, &color[0]));

    // Bind the VAO
    GLCall(glBindVertexArray(VAO));
    //    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
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

    // Bind to the second VBO - We will use it to store the vertex_normals
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_normals));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * vertex_normals.size(), vertex_normals.data(), GL_STATIC_DRAW));
    GLCall(glEnableVertexAttribArray(shader->Normal_vec4));
    GLCall(glVertexAttribPointer(shader->Normal_vec4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0));

    // Bind to the second VBO - We will use it to store the vertex_texcoords
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_texcoords));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertex_texcoords.size(), vertex_texcoords.data(), GL_STATIC_DRAW));
    GLCall(glEnableVertexAttribArray(shader->TexCoord_vec2));
    GLCall(glVertexAttribPointer(shader->TexCoord_vec2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0));
    
    // Generate EBO, bind the EBO to the bound VAO and send the data
    GLCall(glGenBuffers(1, &EBO));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW));
    
    // Unbind the VBOs.
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindVertexArray(0));
	
	GL_ERRORS(); //PARANOIA: make sure nothing strange happened during setup
}


void SquareObject::createVerts(){

    vertex_positions = vector<glm::vec4>({
        glm::vec4(width,  width, 0.0f, 1.0f),  // top right
        glm::vec4(width, -width, 0.0f, 1.0f),  // bottom right
        glm::vec4(-width, -width, 0.0f, 1.0f),  // bottom left
        glm::vec4(-width,  width, 0.0f, 1.0f)  // top left 
    });

    vertex_normals = vector<glm::vec4>({
        glm::normalize(glm::vec4(width,  width, 0.0f, 0.0f)),  // top right
        glm::normalize(glm::vec4(width, -width, 0.0f, 0.0f)),  // bottom right
        glm::normalize(glm::vec4(-width, -width, 0.0f, 0.0f)),  // bottom left
        glm::normalize(glm::vec4(-width,  width, 0.0f, 0.0f))  // top left 
    });

    vertex_texcoords = vector<glm::vec2>({
        glm::vec2(0.f,1.f),
        glm::vec2(1.f,1.f),
        glm::vec2(0.f,0.f),
        glm::vec2(1.f,0.f)
    });
}

void SquareObject::reset(){
    GameObject::reset();
    
    vertex_positions.clear();
    vertex_normals.clear();
}

void SquareObject::setWidth(float r){
    width = r;
    
    vertex_positions.clear();
    vertex_normals.clear();
    
    createVerts();
    prepareDraw();
}
