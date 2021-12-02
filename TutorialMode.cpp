#include "TutorialMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

TutorialMode::TutorialMode() {
    
}

TutorialMode::~TutorialMode() {

}

bool TutorialMode::handle_event(SDL_Event const& evt, glm::uvec2 const& window_size) {
    if (evt.type == SDL_KEYDOWN) {
        if (evt.key.keysym.sym == SDLK_SPACE) {
            Mode::set_current(std::make_shared<PlayMode>());
            return true;
        }
    }
    return false;
}

void TutorialMode::update(float elapsed) {
    elpasedTime += elapsed;
    if (elpasedTime > TUTORIAL_TIME) {
        Mode::set_current(std::make_shared<PlayMode>());
    }
}

void TutorialMode::draw(glm::uvec2 const& drawable_size) {

    glDisable(GL_BLEND);

    // ----- set the background color -----
    glm::uvec4 background_color(37, 12, 10, 0);
    glClearColor(
        background_color.x / 255.0f,  // red
        background_color.y / 255.0f,  // green
        background_color.z / 255.0f,  // blue 
        background_color.w / 255.0f); // alpha
    glClear(GL_COLOR_BUFFER_BIT);
}