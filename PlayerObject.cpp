#include "load_save_png.hpp"
#include "PlayerObject.hpp"
#include "Inivar.hpp"
#include <signal.h>
#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

#define SPEED 10.f

PlayerObject::PlayerObject() {
}

PlayerObject::PlayerObject(float mass, const glm::vec3& pos, glm::uvec2 sz,
    const glm::vec3& vel, bool isFixed, const std::string& filename, float l) :
    GameObject(mass, pos, vel, isFixed, l), size(sz) {

    if (!filename.empty())
        load_png(data_path(filename),
            &(sz), &(pic), OriginLocation::LowerLeftOrigin);
    else {
        pic = vector<glm::u8vec4>(1, glm::u8vec4(0xff));
        sz = glm::uvec2(1, 1);
    }

    createVerts();

    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO_positions));
    GLCall(glGenBuffers(1, &VBO_texcoords));
    GLCall(glGenBuffers(1, &EBO));

    prepareDraw();

    box = std::shared_ptr<PlayerCollisionBox>(new PlayerCollisionBox(position, size, false));
    CollisionSystem::Instance().player1_collision = box;
}

PlayerObject::~PlayerObject() {
    // Delete the VBOs and the VAO.
    glDeleteBuffers(1, &VBO_positions);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void PlayerObject::prepareDraw() {

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
    GLCall(glGenBuffers(1, &EBO));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW));

    // Unbind the VBOs.
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindVertexArray(0));

    GL_ERRORS(); //PARANOIA: make sure nothing strange happened during setup
}

void PlayerObject::createVerts() {

    vertex_positions = vector<glm::vec4>({
        glm::vec4(size.x,  size.y, 0.0f, 1.0f),  // top right
        glm::vec4(size.x, -size.y, 0.0f, 1.0f),  // bottom right
        glm::vec4(-size.x, -size.y, 0.0f, 1.0f),  // bottom left
        glm::vec4(-size.x,  size.y, 0.0f, 1.0f)  // top left 
        });

    vertex_texcoords = vector<glm::vec2>({
        glm::vec2(0.f,1.f),
        glm::vec2(1.f,1.f),
        glm::vec2(0.f,0.f),
        glm::vec2(1.f,0.f)
        });
}

void PlayerObject::reset() {
    GameObject::reset();

    vertex_positions.clear();
}

void PlayerObject::update(float elapsed) {
    std::cout << "update player\n";
    if (left.pressed && !right.pressed) {
        // move left
        glm::vec2 new_pos = glm::vec2{ transform->position.x, transform->position.y };
        new_pos.x -= SPEED * elapsed;
        if (!CollisionSystem::Instance().PlayerCheckCollision(new_pos, glm::vec2{size.x, size.y})) {
            transform->position.x = new_pos.x;
            transform->position.y = new_pos.y;

        }
    }
    else if (!left.pressed && right.pressed) {
        // move right
        glm::vec2 new_pos = glm::vec2{ transform->position.x, transform->position.y };
        new_pos.x += SPEED * elapsed;
        if (!CollisionSystem::Instance().PlayerCheckCollision(new_pos, glm::vec2{size.x, size.y})) {
            transform->position.x = new_pos.x;
            transform->position.y = new_pos.y;
        }
    }

    std::cout << "player position: x: " << transform->position.x << "; y: " << transform->position.y << "\n";
}

void PlayerObject::draw(glm::uvec2 const& drawable_size) {
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

