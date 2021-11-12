#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "Inivar.hpp"
#include "GLCall.hpp"
#include "PlayerStats.hpp"

#include <algorithm>
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
		if (transform.name == "Player1") {
			player1 = new PlayerObject(10.f, glm::vec3(transform.position.x, transform.position.y, 0.f),
				transform.scale.x, transform.scale.y, glm::vec3(0.f, 0.f, 0.f),
				false, "resource/QinYe.png");
		}
		else if (transform.name == "Player2") {
			player2 = new SecondPlayerObject(10.f, glm::vec3(transform.position.x, transform.position.y, 0.f),
				transform.scale.x, transform.scale.y, glm::vec3(0.f, 0.f, 0.f),
				true, "resource/gg.png");
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
		else if (transform.name.find("Collectable") != string::npos) {
			auto collectable = new CollectableObject(10.f, glm::vec3(transform.position.x, transform.position.y, 0.f),
				transform.scale.x, transform.scale.y, glm::vec3(0.f, 0.f, 0.f),
				true, "resource/light.png");

			collectableObjs.push_back(collectable);

			collectable->box = CollisionSystem::Instance().AddOneCollectable(glm::vec2(transform.position.x, transform.position.y),
				glm::vec2(transform.scale.x * 2, transform.scale.y * 2),
				transform.name);
			collectable->box->owner = collectable;
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
	moveableObjs.push_back(player2);
	
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
				return true;
			case SDLK_w:
				if (!evt.key.repeat) {
					player1->space.pressed = true;
					return true;
				}
				player1->space.pressed = false;
				return true;
			case P2_LEFT:
				player2->left.pressed = true;
				return true;
			case P2_RIGHT:
				player2->right.pressed = true;
				return true;
			case P2_UP:
				player2->up.pressed = true;
				return true;
			case P2_DOWN:
				player2->down.pressed = true;
				return true;
			case SDLK_s:
				up.downs += 1;
				up.pressed = true;
				return true;
			case SDLK_q:
				counterClockwiseRot.downs += 1;
				counterClockwiseRot.pressed = true;
				return true;
			case SDLK_e:
				clockwiseRot.downs += 1;
				clockwiseRot.pressed = true;
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
				player1->space.pressed = false;
				return true;
			case P2_LEFT:
				player2->left.pressed = false;
				return true;
			case P2_RIGHT:
				player2->right.pressed = false;
				return true;
			case P2_UP:
				player2->up.pressed = false;
				return true;
			case P2_DOWN:
				player2->down.pressed = false;
				return true;
			case SDLK_s:
				up.downs = 0;
				up.pressed = false;
				return true;
			case SDLK_q:
				counterClockwiseRot.downs = 0;
				counterClockwiseRot.pressed = false;
				return true;
			case SDLK_e:
				clockwiseRot.downs = 0;
				clockwiseRot.pressed = false;
				return true;
			default:
				break;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
		
	// gravity spell
	{
		// camera rotation speed
		float CameraRotSpeed = WORLD_ROT_ANGLE * elapsed;
		if (isGravitySpellLocked) {
			gravitySpellRot -= CameraRotSpeed;
			
			float remain = 0.f;
			float factor = isClockwise ? 1.f : -1.f;

			if (gravitySpellRot < 0.f) {
				// record the remain angle so that 
				// we will not over rotated
				remain = gravitySpellRot;
				gravitySpellRot = WORLD_ROT_ANGLE;
				isGravitySpellLocked = false;
			}

			std::cout << gravitySpellRot << std::endl;
			
			// substract from the rotation angle if over roate,
			// otherwise rotate on z axis
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(
					glm::radians(factor * (CameraRotSpeed + remain)), glm::vec3(0.0f, 0.0f, 1.0f))
			);
			
			for (auto& obj : moveableObjs){
				obj->applyRotation(
					glm::vec3(0.f, 0.f, glm::radians(factor * (CameraRotSpeed + remain))), 
					camera->transform->position);
			}
		}

		if (clockwiseRot.pressed && !isGravitySpellLocked) {
			gravity = glm::vec3(-gravity.y, gravity.x, 0.f);
			isGravitySpellLocked = true;
			isClockwise = true;
			PlayerStats::Instance().rotMat = PlayerStats::Instance().rotMat
				* glm::mat3(
					glm::cos(-glm::radians(WORLD_ROT_ANGLE)), glm::sin(-glm::radians(WORLD_ROT_ANGLE)), 0,
					-glm::sin(-glm::radians(WORLD_ROT_ANGLE)), glm::cos(-glm::radians(WORLD_ROT_ANGLE)), 0,
					0, 0, 0);
		} else if (counterClockwiseRot.pressed && !isGravitySpellLocked) {
			gravity = glm::vec3(gravity.y, -gravity.x, 0.f);
			isGravitySpellLocked = true;
			isClockwise = false;
			PlayerStats::Instance().rotMat = PlayerStats::Instance().rotMat
				* glm::mat3(
					glm::cos(glm::radians(WORLD_ROT_ANGLE)), glm::sin(glm::radians(WORLD_ROT_ANGLE)), 0,
					-glm::sin(glm::radians(WORLD_ROT_ANGLE)), glm::cos(glm::radians(WORLD_ROT_ANGLE)), 0,
					0, 0, 0);
		}
	}

	// force apply test
	{
		for (auto& obj : moveableObjs){
			obj->zeroForce();
			glm::vec3 force = gravity * obj->getMass();
			obj->applyForce(force);
			
			obj->update(elapsed);
		}
	}

	{
		// camera->transform->position = glm::vec3(player1->getPos().x, 
		// 	player1->getPos().y, camera->transform->position.z);

		// // camera movement test
		float CameraSpeed = player1->getSpeed();
		
		glm::vec2 playerPosOnWindow = glm::vec2(
			player1->getPos().x - camera->transform->position.x, 
			player1->getPos().y - camera->transform->position.y);

		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 right = frame[0];
		glm::vec3 up = frame[1];	
		// glm::vec3 forward = -frame[2];

		if (playerPosOnWindow.x > 30.f)
			camera->transform->position = camera->transform->position 
				+ PlayerStats::Instance().rotMat * CameraSpeed * elapsed * right;
		
		if (playerPosOnWindow.x < -30.f)
			camera->transform->position = camera->transform->position 
				- PlayerStats::Instance().rotMat * CameraSpeed * elapsed * right;

		if (playerPosOnWindow.y > 20.f)
			camera->transform->position = camera->transform->position 
				+ PlayerStats::Instance().rotMat * CameraSpeed * elapsed * up;

		if (playerPosOnWindow.y < -20.f)
			camera->transform->position = camera->transform->position 
				- PlayerStats::Instance().rotMat * CameraSpeed * elapsed * up;
		
		//camera->transform->position.x =std::clamp(camera->transform->position.x, 140.0f, 300.0f);
		//camera->transform->position.y = std::clamp(camera->transform->position.y, 100.0f, 240.0f);
	}

	// check reset
	{
		if (player1->getPos().x <= 0.f || player1->getPos().y <= 0.f || PlayerStats::Instance().health <= 0.f) {
			player1->reset();
			player2->reset();
			gravity = glm::vec3(0, -98.f, 0);

			camera->transform->position.x = player1->getPos().x;
			camera->transform->position.y = player1->getPos().y;

			camera->transform->rotation = glm::quat{ 1.f, 0.f, 0.f, 0.f };
			gravitySpellRot = WORLD_ROT_ANGLE;
			isGravitySpellLocked = false;
		}
	}

	PlayerStats::Instance().to_string();
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);
	camera->drawable_size = drawable_size;

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

	std::cout << "drawing collectables" << std::endl;
	int n = (int)collectableObjs.size();
	for (int i = 0; i < n; i++) {
		std::cout << i << std::endl;
		auto obj = collectableObjs[i];
		if (obj->getLife() > 0) {
			obj->draw(*camera);
		}
		else {
			
			obj->~CollectableObject();
			n--;

			std::cout << "erase" << std::endl;
			collectableObjs.erase(collectableObjs.begin() + i);
			
			i--;
		}
	}
	std::cout << "finished drawing collectables" << std::endl;

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDisable(GL_BLEND));
	GLCall(glDepthMask(GL_TRUE));
	GL_ERRORS();
}
