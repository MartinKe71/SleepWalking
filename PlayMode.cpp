#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <random>

PlayMode::PlayMode() {
	moveableObjs.push_back(new SquareObject(10.f, 
		glm::vec3(0.0f, 0.5f, 0.f), glm::vec3(1.f, 0.f, 0.f), false, 0.05f, "resource/mos.png"));
	moveableObjs.push_back(new SquareObject(10.f, 
		glm::vec3(0.5f, 0.5f, 0.f), glm::vec3(1.f, 0.f, 0.f), false, 0.05f, "resource/mos.png"));
	moveableObjs.push_back(new SquareObject(10.f, 
		glm::vec3(-0.5f, 0.5f, 0.f), glm::vec3(1.f, 0.f, 0.f), true, 0.05f, "resource/mos.png"));
}

PlayMode::~PlayMode() {
	for (auto& obj : moveableObjs){
        delete obj;
    }
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		mouse_pos = glm::vec2(
			evt.motion.x / float(window_size.x) * 2 - 1.0f,
			-evt.motion.y / float(window_size.y) * 2 + 1.0f
		);
		return true;
	}

	return false;
}

void PlayMode::update(float elapsed) {

	{
		if (down.pressed && !up.pressed) gravity = glm::vec3 (0, -9.8f, 0);
		if (!down.pressed && up.pressed) gravity  = glm::vec3 (0, 9.8f, 0);
	}

	{
		for (auto& obj : moveableObjs){
			obj->zeroForce();
			glm::vec3 force = gravity * obj->getMass();
			obj->applyForce(force);
			
			obj->update(elapsed);
		}
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto& obj : moveableObjs){
		obj->draw(drawable_size);
	}
	GL_ERRORS();
}
