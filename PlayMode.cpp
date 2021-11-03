#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "Inivar.hpp"

#include <fstream>
#include <random>

#include <signal.h>
#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

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

GLuint sleepWalking_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > sleepWalking_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("SleepWalking.pnct"));
	sleepWalking_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > sleepWalking_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("SleepWalking.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = sleepWalking_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = sleepWalking_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

PlayMode::PlayMode() : scene(*sleepWalking_scene){

	// for (auto &transform : scene.transforms) {
	// 	if (transform.name.rfind("eggPlate", 0) == 0) {
	// 		std::size_t found = transform.name.find_first_of(".");
	// 		int idx = std::stoi(transform.name.substr(found + 1));
	// 		eggPlates[idx - 1] = &transform;
	// 	} else if (transform.name == "Marker") marker = &transform;
		
	// }

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) 
		throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();
	
	moveableObjs.push_back(new SquareObject(10.f, 
		glm::vec3(-10.0f, 1.0f, 0.f), glm::vec3(1.f, 0.f, 0.f), false, 1.f, "resource/mos.png"));
	moveableObjs.push_back(new SquareObject(10.f, 
		glm::vec3(10.0f, 1.0f, 0.f), glm::vec3(1.f, 0.f, 0.f), false, 1.f, "resource/mos.png"));
	moveableObjs.push_back(new SquareObject(10.f, 
		glm::vec3(20.0f, 1.0f, 0.f), glm::vec3(1.f, 0.f, 0.f), true, 1.f, "resource/mos.png"));
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
	} else if (evt.key.keysym.sym == P1_LEFT) {
			player1.left.pressed = true;
			return true;
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
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	GLCall(glUseProgram(lit_color_texture_program->program));
	GLCall(glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1));
	GLCall(glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f))));
	GLCall(glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f))));
	GLCall(glUseProgram(0));

	GLCall(glClearColor(0.5f, 0.5f, 0.5f, 1.0f));
	GLCall(glClearDepth(1.0f)); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	// glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	GLCall(glDisable(GL_DEPTH_TEST));

	GLCall(glDepthMask(GL_FALSE));

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	scene.draw(*camera);

	for (auto& obj : moveableObjs){
		obj->draw(*camera);
	}

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDisable(GL_BLEND));
	GLCall(glDepthMask(GL_TRUE));
	GL_ERRORS();
}
