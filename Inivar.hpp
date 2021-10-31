#include "GL.hpp"
#include "gl_errors.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>

#define P1_LEFT SDLK_a
#define P1_RIGHT SDLK_d
#define P1_UP SDLK_SPACE
#define P2_LEFT SDLK_LEFT
#define P2_RIGHT SDLK_RIGHT
#define P2_UP SDLK_UP
#define P2_DOWN SDLK_DOWN