#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "Inivar.hpp"
#include "GLCall.hpp"

#include <fstream>
#include <random>

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

	for (auto& transform : scene.transforms) {
		if (transform.name == "Player") {
			player1 = new PlayerObject(10.f, glm::vec3(transform.position.x, transform.position.y, 0.f),
				transform.scale.x, transform.scale.y, glm::vec3(0.f, 0.f, 0.f),
				false, "resource/blood32.png");
		}
		else if (transform.name.find("Block") != string::npos) {
			CollisionSystem::Instance().AddOneSceneBlock(glm::vec2(transform.position.x, transform.position.y), 
				glm::vec2(transform.scale.x * 2, transform.scale.y * 2),
				transform.name);
		}
		else if (transform.name.find("Trigger") != string::npos) {
			CollisionSystem::Instance().AddOneThornBlock(glm::vec2(transform.position.x, transform.position.y), 
				glm::vec2(transform.scale.x * 2, transform.scale.y * 2),
				transform.name);
		}
	}

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) 
		throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();
	
	// moveableObjs.push_back(new SquareObject(10.f, 
	// 	glm::vec3(50.0f, 50.0f, 0.f), glm::vec3(1.f, 0.f, 0.f), false, 3.f, "resource/blood32.png"));
	//moveableObjs.push_back(new SquareObject(10.f, 
	//	glm::vec3(60.0f, 50.0f, 0.f), glm::vec3(1.f, 0.f, 0.f), false, 3.f, "resource/flyswatter32.png"));
	//moveableObjs.push_back(new SquareObject(10.f, 
	//	glm::vec3(50.0f, 40.0f, 0.f), glm::vec3(1.f, 0.f, 0.f), true, 3.f, "resource/mos.png"));
	
	moveableObjs.push_back(player1);
	
}

PlayMode::~PlayMode() {
	for (auto& obj : moveableObjs){
        delete obj;
    }
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
			case P1_UP:
				if (!evt.key.repeat) {
					player1->space.pressed = true;
					return true;
				}
				player1->space.pressed = false;
				return false;
			case SDLK_w:
				up.downs += 1;
				up.pressed = true;
				return true;
			case SDLK_s:
				up.downs += 1;
				up.pressed = true;
				return true;
			case SDLK_r:
				gravitySpell.downs += 1;
				gravitySpell.pressed = true;
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
			case P1_UP:
				player1->space.pressed = false;
				return true;
			case SDLK_w:
				up.downs = 0;
				up.pressed = false;
				return true;
			case SDLK_s:
				up.downs = 0;
				up.pressed = false;
				return true;
			case SDLK_r:
				gravitySpell.downs = 0;
				gravitySpell.pressed = false;
				return true;
			default:
				break;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	// gravity test
	{
		// camera rotation speed
		float CameraRotSpeed = 180.0f * elapsed;
		if (isGravitySpellLocked) {
			gravitySpellRot -= CameraRotSpeed;
			
			float remain = 0.f;
			if (gravitySpellRot < 0.f) {
				// record the remain angle so that 
				// we will not over rotated
				remain = gravitySpellRot;
				gravitySpellRot = 180.f;
				isGravitySpellLocked = false;
				
			}

			// substract from the rotation angle if over roate,
			// otherwise rotate on z axis
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(glm::radians(CameraRotSpeed + remain), glm::vec3(0.0f, 0.0f, 1.0f))
			);
			
			for (auto& obj : moveableObjs){
				obj->applyRotation(
					glm::vec3(0.f, 0.f, glm::radians(CameraRotSpeed + remain)), 
					camera->transform->position);
			}
		}
		if (gravitySpell.pressed && !isGravitySpellLocked) {
			gravity = -gravity;
			isGravitySpellLocked = true;
			player1->direction *= -1;
		}
	}

	// camera movement test
	//{
	//	constexpr float CameraSpeed = 10.0f;
	//	glm::mat4x3 frame = camera->transform->make_local_to_parent();
	//	glm::vec3 right = frame[0];
	//	glm::vec3 up = frame[1];
	//	// glm::vec3 forward = -frame[2];

	//	if (mouse_pos.x > 0.9)
	//		camera->transform->position = camera->transform->position + CameraSpeed * elapsed * right;
	//	
	//	if (mouse_pos.x < -0.9)
	//		camera->transform->position = camera->transform->position - CameraSpeed * elapsed * right;

	//	if (mouse_pos.y > 0.9)
	//		camera->transform->position = camera->transform->position + CameraSpeed * elapsed * up;

	//	if (mouse_pos.y < -0.9)
	//		camera->transform->position = camera->transform->position - CameraSpeed * elapsed * up;
	//}

	// force apply test
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
	scene.draw(*camera);
	GLCall(glDisable(GL_DEPTH_TEST));

	GLCall(glDepthMask(GL_FALSE));

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	

	for (auto& obj : moveableObjs){
		obj->draw(*camera);
	}

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDisable(GL_BLEND));
	GLCall(glDepthMask(GL_TRUE));
	GL_ERRORS();
}
