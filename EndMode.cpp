#include "EndMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

EndMode::EndMode() {
    ending = new UIBGObject(glm::vec3(0.f, 0.0f, 0.f), 2.f, 2.f, "resource/Ending.png");
}

EndMode::~EndMode() {

}

bool EndMode::handle_event(SDL_Event const& evt, glm::uvec2 const& window_size) {
    
    return false;
}

void EndMode::update(float elapsed) {
    
}

void EndMode::draw(glm::uvec2 const& drawable_size) {

    glDisable(GL_BLEND);

    // ----- set the background color -----
    glm::uvec4 background_color(1, 1, 1, 0);
    glClearColor(
        background_color.x / 255.0f,  // red
        background_color.y / 255.0f,  // green
        background_color.z / 255.0f,  // blue 
        background_color.w / 255.0f); // alpha
    glClear(GL_COLOR_BUFFER_BIT);

    ending->draw(*camera);
}