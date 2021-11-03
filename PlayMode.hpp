#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "FontSource.hpp"
#include "data_path.hpp"
#include "SquareObject.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;


	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;
	//camera:
	Scene::Camera *camera = nullptr;
	glm::vec2 mouse_pos;

	//----- game physics -----
	glm::vec3 gravity = glm::vec3 (0, -9.8f, 0);

	//----- game object -----
	vector<GameObject*> moveableObjs;

	//----- game state -----
	float gravitySpellRot = 180.f;
	bool isGravitySpellLocked = false;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, gravitySpell;

	struct Player1 {
		glm::vec3 pos;
		Button left, right, up;
	} player1;

	struct Player2 {
		glm::vec3 pos;
		Button left, right, up, down;
	} player2;
};
