#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "Inivar.hpp"

#include <fstream>
#include <random>

PlayMode::PlayMode() {
	moveableObjs.push_back(new SquareObject(10.f, 
		glm::vec3(0.0f, 0.5f, 0.f), glm::vec3(1.f, 0.f, 0.f), false, 0.05f, "resource/mos.png"));
	moveableObjs.push_back(new SquareObject(10.f, 
		glm::vec3(0.5f, 0.5f, 0.f), glm::vec3(1.f, 0.f, 0.f), false, 0.05f, "resource/mos.png"));
	moveableObjs.push_back(new SquareObject(10.f, 
		glm::vec3(-0.5f, 0.5f, 0.f), glm::vec3(1.f, 0.f, 0.f), true, 0.05f, "resource/mos.png"));
	
	player1 = new PlayerObject(10.f, glm::vec3(0.0f, 0.5f, 0.f),
		glm::uvec2(0.05f, 0.1f), glm::vec3(0.f, 0.f, 0.f),
		true, "resource/mos.png");
	
	moveableObjs.push_back(player1);
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {
		switch (evt.key.keysym.sym) {
			case P1_LEFT:
				player1->left.pressed = true;
				return true;
			case P1_RIGHT:
				player1->right.pressed = true;
				return true;
			default:
				break;
		}
	} else if (evt.type == SDL_KEYUP) {
		switch (evt.key.keysym.sym) {
			case P1_LEFT:
				player1->left.pressed = false;
				return true;
			case P1_RIGHT:
				player1->right.pressed = false;
				return true;
			default:
				break;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GL_ERRORS();
}
