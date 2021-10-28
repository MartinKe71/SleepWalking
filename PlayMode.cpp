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
	// Set up textblock vector
	{
		// Start Button
		blocks.emplace_back(TextBlock("Pacifico.ttf",
							true, true,
							80,
							{ -0.2f, 0.4f },
							{ 0.4f, 0.2f },
							{ 0.03f, 0.03f },
							{ 0x00, 0x00, 0x00, 0xff },
							{ 0xff, 0xff, 0xff, 0x00 },
							"Start"
		));

		// Customer Description
		blocks.emplace_back(TextBlock("Dimbo.ttf",
			true, false,
			24,
			{ -0.77f, -0.36f },
			{ 1.54f, 0.5f },
			{ 0.06f, 0.05f },
			{ 0x00, 0x00, 0x00, 0xff },
			{ 0xe0, 0xe0, 0xe0, 0xff },
			"This is just a sample of what a customer description is going to be like for real blah\nnothing special"
		));

		// Customer Name tag
		blocks.emplace_back(TextBlock("Pacifico.ttf",
			true, false,
			28,
			{ -0.75f, -0.2f },
			{ 0.13f, 0.09f },
			{ 0.02f, 0.02f },
			{ 0x00, 0x00, 0x00, 0xff },
			{ 0xe0, 0xe0, 0xe0, 0xff },
			"Jerry"
		));

		// Total Profit
		blocks.emplace_back(TextBlock("Pacifico.ttf",
			true, false,
			18,
			{ -0.95f, 0.93f },
			{ 0.4f, 0.08f },
			{ 0.02f, 0.02f },
			{ 0x00, 0x00, 0x00, 0xff },
			{ 0xe0, 0xe0, 0xe0, 0xff },
			"Current Profit : $200 / $1000"
		));

		// Choice Option 0
		blocks.emplace_back(TextBlock("Dimbo.ttf",
			true, true,
			28,
			{ 0.5f, 0.7f },
			{ 0.3f, 0.1f },
			{ 0.02f, 0.02f },
			{ 0x00, 0x00, 0x00, 0xff },
			{ 0xe0, 0xe0, 0xe0, 0xff },
			"$150"
		));

		// Choice Option 1
		blocks.emplace_back(TextBlock("Dimbo.ttf",
			true, true,
			28,
			{ 0.5f, 0.5f },
			{ 0.3f, 0.1f },
			{ 0.02f, 0.02f },
			{ 0x00, 0x00, 0x00, 0xff },
			{ 0xe0, 0xe0, 0xe0, 0xff },
			"$150"
		));

		// Choice Option 2
		blocks.emplace_back(TextBlock("Dimbo.ttf",
			true, true,
			28,
			{ 0.5f, 0.3f },
			{ 0.3f, 0.1f },
			{ 0.02f, 0.02f },
			{ 0x00, 0x00, 0x00, 0xff },
			{ 0xe0, 0xe0, 0xe0, 0xff },
			"$150"
		));

		// Choice Option 3
		blocks.emplace_back(TextBlock("Dimbo.ttf",
			true, true,
			28,
			{ 0.5f, 0.1f },
			{ 0.3f, 0.1f },
			{ 0.02f, 0.02f },
			{ 0x00, 0x00, 0x00, 0xff },
			{ 0xe0, 0xe0, 0xe0, 0xff },
			"$150"
		));

		// Choice Option 4
		blocks.emplace_back(TextBlock("Dimbo.ttf",
			true, true,
			28,
			{ 0.5f, -0.1f },
			{ 0.3f, 0.1f },
			{ 0.02f, 0.02f },
			{ 0x00, 0x00, 0x00, 0xff },
			{ 0xe0, 0xe0, 0xe0, 0xff },
			"$150"
		));

		// Num Customer Left
		blocks.emplace_back(TextBlock("Dimbo.ttf",
			true, false,
			18,
			{ 0.75f, 0.93f },
			{ 0.2f, 0.08f },
			{ 0.02f, 0.02f },
			{ 0x00, 0x00, 0x00, 0xff },
			{ 0xe0, 0xe0, 0xe0, 0xff },
			"Customer Left: 3"
		));	
	}

	// Only show start button at launch
	{
		for (size_t i = 1; i < blocks.size(); i++) {
			blocks[i].visible = false;
		}
	}

	fsm = FSM();
	fsm.init(blocks);
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_MOUSEMOTION) {
		mouse_pos = glm::vec2(
			evt.motion.x / float(window_size.x) * 2 - 1.0f,
			-evt.motion.y / float(window_size.y) * 2 + 1.0f
		);

		//std::cout << "Mouse motion: x: " << mouse_pos.x << ", y: " << mouse_pos.y << "\n";
		CheckMouseHover();
		return true;
	}
	else if (evt.type == SDL_MOUSEBUTTONUP) {
		if (evt.button.button == SDL_BUTTON_LEFT && selected_block_idx > -1) {
			int action = selected_block_idx > 0 ? selected_block_idx - 3 : selected_block_idx;
			fsm.transferState(action);
			fsm.executeState(blocks);
		}
	}

	return false;
}

void PlayMode::CheckMouseHover() {
	for (auto& block : blocks) {
		if (!block.visible || !block.interactable)
			continue;

		if (mouse_pos.x > block.anchor.x && mouse_pos.x < block.anchor.x + block.dims.x
			&& mouse_pos.y < block.anchor.y && mouse_pos.y > block.anchor.y - block.dims.y) {
			selected_block_idx = static_cast<int>(&block - &blocks[0]);
			return;
		}
	}

	selected_block_idx = -1;
}

void PlayMode::update(float elapsed) {
	
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	for (auto& block : blocks) {
		if (block.visible) {
			if (selected_block_idx > -1 && &block - &blocks[0] == selected_block_idx) {
				//shape_texture_program->DrawBox(block.box);
				shape_texture_program->SetBoxHighlight(block.box, block.box_size);
				block.source->DrawText(drawable_size, block.text, block.anchor, block.dims, { 0xff, 0xff, 0xff, 0xff });
			}
			else {
				//shape_texture_program->DrawBox(block.box);
				shape_texture_program->ResetBoxHighlight(block.box, block.box_size);
				block.source->DrawText(drawable_size, block.text, block.anchor, block.dims, block.font_color);
			}			
		}
	}
	GL_ERRORS();
}
