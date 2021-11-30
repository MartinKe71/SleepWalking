#pragma once

#include "GL.hpp"
#include "gl_errors.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "data_path.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <memory>

enum class AudioSourceList 
{
	// Short sound
	Jump,
	Rotate,
	Drag,
	Die,
	CheckPoint,
	Collect,

	// Long sound
	Footsteps,
	Timestop,
	BGM
};

#define GAME_MAP_SIZE glm::vec2(1024.f * 4, 416.5f * 4)
#define GAME_MAP_ORIGIN glm::vec2(0.f, 0.f)
#define CAM_OFFSET glm::vec2(30.f, 20.f)

#define CAMERA_SPEED 30.f
#define COLLISION_OPT_LEVEL 4

#define WORLD_ROT_ANGLE 90.f
#define HALF_VOLUME_RADIUS 10.0f
#define TIMESTOP_TIMER 3.f
#define TIMESTOP_CD 10.f
#define DRAG_FORCE_MAG 1000.f
#define DRAG_CD 5.f
#define DRAG_TIMER 0.5f
#define MARKER_FADE_OUT 0.02f


// key bindings
#define P1_LEFT SDLK_a
#define P1_RIGHT SDLK_d
#define P1_UP SDLK_SPACE
#define P2_LEFT SDLK_LEFT
#define P2_RIGHT SDLK_RIGHT
#define P2_UP SDLK_UP
#define P2_DOWN SDLK_DOWN
#define KEY_DRAG SDLK_PERIOD
#define KEY_TIMESTOP SDLK_SLASH
#define KEY_CLOCK_ROT SDLK_e
#define KEY_COUNTERCLOCK_ROT SDLK_q
#define KEY_FLIP SDLK_r

#define LIGHT_DECAY 0.12f
#define MAX_LIGHT_AMOUNT 10.f