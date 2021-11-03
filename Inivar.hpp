#pragma once

#include "GL.hpp"
#include "gl_errors.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>

#include <signal.h>
#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

#define GAME_MAP_SIZE glm::vec2(192.f, 108.f)
#define COLLISION_OPT_LEVEL 4

#define P1_LEFT SDLK_a
#define P1_RIGHT SDLK_d
#define P1_UP SDLK_SPACE
#define P2_LEFT SDLK_LEFT
#define P2_RIGHT SDLK_RIGHT
#define P2_UP SDLK_UP
#define P2_DOWN SDLK_DOWN

#ifdef _MSC_VER
#define DEBUG_BREAK __debugbreak()
#elif __APPLE__
#define DEBUG_BREAK __builtin_trap()
#else
#define DEBUG_BREAK raise(SIGTRAP)
#endif

#define ASSERT(x) if (!(x)) DEBUG_BREAK;
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] ( " << error << " ) : " << function << " " << file << ": " << line << std::endl;
		return false;
	}
	return true;
}