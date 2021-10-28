#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "FontSource.hpp"
#include "data_path.hpp"
#include "FSM.hpp"

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

	//----- game state -----
	std::vector<TextBlock> blocks;
	glm::vec2 mouse_pos;
	int selected_block_idx = -1;
	FSM fsm;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	void CheckMouseHover();
};
