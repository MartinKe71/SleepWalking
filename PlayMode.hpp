#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "FontSource.hpp"
#include "data_path.hpp"
#include "SquareObject.hpp"
#include "PlayerObject.hpp"
#include "SecondPlayerObject.hpp"
#include "CollectableObject.hpp"
#include "MovingThornObject.hpp"
#include "MovingBlockObject.hpp"

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
	glm::vec3 gravity = glm::vec3 (0, -98.f, 0);

	//----- game object -----
	vector<GameObject*> moveableObjs;
	vector<CollectableObject*> collectableObjs;

	//----- game state -----
	float gravitySpellRot = WORLD_ROT_ANGLE;
	bool isClockwise = false; 
	bool isGravitySpellLocked = false;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, clockwiseRot, counterClockwiseRot;

	PlayerObject* player1;
	SecondPlayerObject* player2;
	
};
