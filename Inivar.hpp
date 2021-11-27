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

	// Long sound
	Footsteps
};

#define GAME_MAP_SIZE glm::vec2(1536.f * 2, 864.f * 2)
#define GAME_MAP_ORIGIN glm::vec2(0.f, 0.f)
#define CAMERA_SPEED 30.f
#define COLLISION_OPT_LEVEL 2
#define WORLD_ROT_ANGLE 90.f
#define HALF_VOLUME_RADIUS 10.0f

#define P1_LEFT SDLK_a
#define P1_RIGHT SDLK_d
#define P1_UP SDLK_SPACE
#define P2_LEFT SDLK_LEFT
#define P2_RIGHT SDLK_RIGHT
#define P2_UP SDLK_UP
#define P2_DOWN SDLK_DOWN

#define LIGHT_DECAY 0.4f