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
			player1 = new PlayerObject(10.f, transform.position,
				transform.scale.x, transform.scale.y, glm::vec3(0.f, 0.f, 0.f),
				false, "resource/qinye/qinye.png");
			player1->addAnimation("Idle", "resource/qinye/idle.txt");
			player1->addAnimation("Run", "resource/qinye/walk.txt");
		}
		else if (transform.name == "Player2") {
			player2 = new SecondPlayerObject(10.f, transform.position,
				transform.scale.x, transform.scale.y, glm::vec3(0.f, 0.f, 0.f),
				true, "resource/elf.png");
		}
		else if (transform.name.find("Block") != string::npos) {
			if (transform.name.find("Block_S") != string::npos) {
				CollisionSystem::Instance().AddOneSceneBlock(glm::vec2(transform.position.x, transform.position.y),
					glm::vec2(transform.scale.x * 2, transform.scale.y * 2),
					transform.name);
			}
			else if (transform.name.find("Block_M") != string::npos) {
				istringstream stream(transform.name);
				vector<string> properties;
				string token;
				while (getline(stream, token, '_')) properties.push_back(token);

				if (properties.size() < 5) throw std::runtime_error("Invalid Moving Block Name: " + transform.name);

				auto obj = new MovingBlockObject(10.f, properties, transform.position,
					transform.scale.x, transform.scale.y, glm::vec3(0.f, 0.f, 0.f),
					true, "resource/frame.png", transform.name);

				moveableObjs.push_back(obj);
			}
		}
		else if (transform.name.find("Trigger") != string::npos) {
			if (transform.name.find("Trigger_S") != string::npos) {
				CollisionSystem::Instance().AddOneThornBlock(glm::vec2(transform.position.x, transform.position.y),
					glm::vec2(transform.scale.x * 2, transform.scale.y * 2),
					transform.name);
			}
			else if (transform.name.find("Trigger_M") != string::npos) {
				auto pos = transform.name.find('.');

				istringstream stream(transform.name.substr(0, pos));
				vector<string> properties;
				string token;
				while (getline(stream, token, '_')) properties.push_back(token);

				if (properties.size() < 5) throw std::runtime_error("Invalid Moving Trigger Name: " + transform.name);

				auto obj = new MovingThornObject(10.f, properties, transform.position,
					transform.scale.x, transform.scale.y, glm::vec3(0.f, 0.f, 0.f),
					true, "resource/moving_throne.png", transform.name);

				moveableObjs.push_back(obj);
			}			
		}
		else if (transform.name.find("Collectable") != string::npos) {
			auto collectable = new CollectableObject(10.f, glm::vec3(transform.position.x, transform.position.y, 0.f),
				transform.scale.x, transform.scale.y, glm::vec3(0.f, 0.f, 0.f),
				true, "resource/gg.png");

			collectableObjs.push_back(collectable);
			//std::cout << "Collectable created with w: " << transform.scale.x << ", and height: " << transform.scale.y << std::endl;

			collectable->box = CollisionSystem::Instance().AddOneCollectable(glm::vec2(transform.position.x, transform.position.y),
				glm::vec2(transform.scale.x * 2, transform.scale.y * 2),
				transform.name);
			collectable->box->owner = collectable;
		}
		else if (transform.name.find("SavePoint") != string::npos) {
			size_t w_i = transform.name.find("_W_");
			size_t h_i = transform.name.find("_H_");
			int w = 0;
			int h = 0;
			if (w_i != string::npos && h_i != string::npos)
			{
				w = std::stoi(transform.name.substr(w_i+3, h_i-w_i-3));
				h = std::stoi(transform.name.substr(h_i+3));
			}
			else
			{
				std::cerr << "SavePoint load failed, string name not valid." << std::endl;
				continue;
			}
			//std::cout << "Save point created with w: " << w << ", and height: " << h << std::endl;
			auto savePoint = new SavePointObject(10.f, glm::vec3(transform.position.x, transform.position.y, 0.f),
				transform.scale.x, transform.scale.y, glm::vec3(0.f, 0.f, 0.f),
				true, "resource/save_point.png");
			
			savePointObjs.push_back(savePoint);

			savePoint->box = CollisionSystem::Instance().AddOneSavePoint(glm::vec2(transform.position.x, transform.position.y),
				glm::vec2(w * 2, h * 2),
				transform.name);

			savePoint->box->owner = savePoint;
		}
	}

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) 
		throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();
	
	markerObjs.push_back(new SquareObject(10.f, 
		glm::vec3(0.0f, 0.0f, 0.f), glm::vec3(1.f, 0.f, 0.f), false, 5.f, "resource/stopwatch.png"));

	markerObjs.push_back(new SquareObject(10.f, 
		glm::vec3(0.0f, 0.0f, 0.f), glm::vec3(1.f, 0.f, 0.f), false, 5.f, "resource/rope.png"));

	playerObjs.push_back(player1);
	playerObjs.push_back(player2);
	
	// moveableObjs.push_back(player1);
	// moveableObjs.push_back(player2);

	hb1 = new HealthBarObject(5.f);
}

PlayMode::~PlayMode() {
	for (auto& obj : moveableObjs){
        delete obj;
    }
	// delete test;
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
			case SDLK_r:
				flip.pressed = true;
				break;
			case SDLK_SLASH:
				timestop.pressed = true;
				break;
			case SDLK_PERIOD:
				drag.pressed = true;
				break;
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
			case SDLK_r:
				flip.pressed = false;
				break;
			case SDLK_SLASH:
				timestop.pressed = false;
				break;
			case SDLK_PERIOD:
				drag.pressed = false;
				break;
			default:
				break;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	for (auto& obj : moveableObjs) obj->zeroForce();
	for (auto& obj : playerObjs) obj->zeroForce();

	// gravity spell
	{
		// camera rotation speed
		float CameraRotSpeed = rotAngle * elapsed;
		if (isGravitySpellLocked) {
			gravitySpellRot -= CameraRotSpeed;
			
			float remain = 0.f;
			float factor = isClockwise ? 1.f : -1.f;

			if (gravitySpellRot < 0.f) {
				// record the remain angle so that 
				// we will not over rotated
				remain = gravitySpellRot;
				gravitySpellRot = rotAngle;
				isGravitySpellLocked = false;
			}

			//std::cout << gravitySpellRot << std::endl;
			
			// substract from the rotation angle if over roate,
			// otherwise rotate on z axis
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(
					glm::radians(factor * (CameraRotSpeed + remain)), glm::vec3(0.0f, 0.0f, 1.0f))
			);
			
			player1->applyRotation(
				glm::vec3(0.f, 0.f, glm::radians(factor * (CameraRotSpeed + remain))),
				camera->transform->position);
			
			player2->applyRotation(
				glm::vec3(0.f, 0.f, glm::radians(factor * (CameraRotSpeed + remain))),
				camera->transform->position);

			//for (auto& obj : moveableObjs){
			//	obj->applyRotation(
			//		glm::vec3(0.f, 0.f, glm::radians(factor * (CameraRotSpeed + remain))), 
			//		camera->transform->position);
			//}
		}

		if (flip.pressed && !isGravitySpellLocked) {
			gravity *= -1.0f;
			isGravitySpellLocked = true;
			isClockwise = !isClockwise;
			rotAngle = WORLD_ROT_ANGLE * 2;
			gravitySpellRot = rotAngle;
			PlayerStats::Instance().rotMat = PlayerStats::Instance().rotMat
				* glm::mat3(
					glm::cos(-glm::radians(rotAngle)), glm::sin(-glm::radians(rotAngle)), 0,
					-glm::sin(-glm::radians(rotAngle)), glm::cos(-glm::radians(rotAngle)), 0,
					0, 0, 0);
		}
		else if (clockwiseRot.pressed && !isGravitySpellLocked) {
			gravity = glm::vec3(-gravity.y, gravity.x, 0.f);
			isGravitySpellLocked = true;
			isClockwise = true;
			rotAngle = WORLD_ROT_ANGLE;
			gravitySpellRot = rotAngle;
			PlayerStats::Instance().rotMat = PlayerStats::Instance().rotMat
				* glm::mat3(
					glm::cos(-glm::radians(rotAngle)), glm::sin(-glm::radians(rotAngle)), 0,
					-glm::sin(-glm::radians(rotAngle)), glm::cos(-glm::radians(rotAngle)), 0,
					0, 0, 0);
		} else if (counterClockwiseRot.pressed && !isGravitySpellLocked) {
			gravity = glm::vec3(gravity.y, -gravity.x, 0.f);
			isGravitySpellLocked = true;
			isClockwise = false;
			rotAngle = WORLD_ROT_ANGLE;
			gravitySpellRot = rotAngle;
			PlayerStats::Instance().rotMat = PlayerStats::Instance().rotMat
				* glm::mat3(
					glm::cos(glm::radians(rotAngle)), glm::sin(glm::radians(rotAngle)), 0,
					-glm::sin(glm::radians(rotAngle)), glm::cos(glm::radians(rotAngle)), 0,
					0, 0, 0);
		}
	}
	// drag spell
	{
		float cd = DRAG_CD;
		float timer = DRAG_TIMER;
		float force_mag = DRAG_FORCE_MAG;
		glm::vec3 force_dir = glm::normalize(player2->getPos() - player1->getPos());
		if (drag.pressed && dragLock == 0.f){
			markerObjs[1]->show(player2->getPos());
			glm::vec3 force = force_mag * force_dir;
			player1->applyForce(force);
			dragLock += elapsed;
			dragTimer += elapsed;
		}
		
		if (dragLock > 0.f) dragLock += elapsed;
		if (dragLock > cd) dragLock = 0.f;
		if (dragTimer > 0.f) dragTimer += elapsed;
		if (dragTimer > timer) dragTimer = 0.f;

		if (dragTimer > 0.f){
			glm::vec3 force = force_mag * force_dir;
			player1->applyForce(force);
		}
	}

	// Check if the player is already in the wall
	CollisionSystem::Instance().update(elapsed);

	// update and time stop
	{
		float cd = TIMESTOP_CD;
		float timer = TIMESTOP_TIMER;
		// active phrase
		if (timestop.pressed && timestopLock == 0.f){
			markerObjs[0]->show(player2->getPos());
			timestopTimer += elapsed;
			timestopLock += elapsed;
		}
		
		// counting phrase
		// if cd set, then keep increment
		if (timestopLock > 0.f) timestopLock += elapsed; 
		if (timestopLock > cd) timestopLock = 0.f;
		if (timestopTimer > 0.f) timestopTimer += elapsed;
		if (timestopTimer > timer) timestopTimer = 0.f;

		// normal phrase
		if (timestopTimer == 0.f){
			for (auto& obj : moveableObjs){
				glm::vec3 force = gravity * obj->getMass();
				obj->applyForce(force);
				obj->update(elapsed);
			}
		}
	}

	{
		for (auto& obj : playerObjs){
			glm::vec3 force = gravity * obj->getMass();
			obj->applyForce(force);
			obj->update(elapsed);
		}
		// std::cout << player1->getForce().x << " " 
		// 	<< player1->getForce().y << " " << player1->getForce().z << std::endl;
		for (auto& obj : markerObjs) obj->fadeOut();
	}

	// camera movement
	{
		// camera->transform->position = glm::vec3(player1->getPos().x, 
		// 	player1->getPos().y, camera->transform->position.z);
		
		// // camera movement test
		float CameraSpeed = max(glm::length(player1->getVelocity()), CAMERA_SPEED);
		
		glm::vec2 offset = CAM_OFFSET;
		offset = glm::vec2(glm::abs(PlayerStats::Instance().rotMat * glm::vec3(offset , 0.f)));

		glm::vec2 playerPosOnWindow = glm::vec2(
			player1->getPos().x - camera->transform->position.x, 
			player1->getPos().y - camera->transform->position.y);
		
		// glm::vec2 player2PosOnWindow = glm::vec2(
		// 	player2->getPos().x - camera->transform->position.x, 
		// 	player2->getPos().y - camera->transform->position.y);

		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 right = frame[0];
		glm::vec3 up = frame[1];	
		// glm::vec3 forward = -frame[2];

		glm::vec3 next_cam_pos = camera->transform->position;
		if (playerPosOnWindow.x > offset.x)
			next_cam_pos = next_cam_pos 
				+ PlayerStats::Instance().rotMat * CameraSpeed * elapsed * right;
		
		if (playerPosOnWindow.x < -offset.x)
			next_cam_pos = next_cam_pos 
				- PlayerStats::Instance().rotMat * CameraSpeed * elapsed * right;

		if (playerPosOnWindow.y > offset.y)
			next_cam_pos = next_cam_pos 
				+ PlayerStats::Instance().rotMat * CameraSpeed * elapsed * up;

		if (playerPosOnWindow.y < -offset.y)
			next_cam_pos = next_cam_pos 
				- PlayerStats::Instance().rotMat * CameraSpeed * elapsed * up;
		
		float cam_h = glm::tan(camera->fovy / 2.f) * camera->transform->position.z;
		float cam_w = camera->aspect * cam_h ;
		glm::vec2 cam_dim = glm::vec2(cam_w, cam_h);
		glm::vec2 map_dim = GAME_MAP_SIZE;

		cam_dim = glm::vec2(PlayerStats::Instance().rotMat * glm::vec3(cam_dim, 0.f));
		map_dim = glm::vec2(PlayerStats::Instance().rotMat * glm::vec3(map_dim, 0.f));

		cam_dim = glm::abs(cam_dim);
		map_dim = glm::abs(map_dim);

		next_cam_pos.x = max(min(next_cam_pos.x, map_dim.x - cam_dim.x), cam_dim.x);
		next_cam_pos.y = max(min(next_cam_pos.y, map_dim.y - cam_dim.y), cam_dim.y);

		camera->transform->position = next_cam_pos;

		glm::vec3 player2_pos = player2->getPos();
		player2_pos.x = max(min(player2_pos.x, 
			camera->transform->position.x + cam_dim.x - offset.x), 
			camera->transform->position.x - cam_dim.x + offset.x);
		player2_pos.y = max(min(player2_pos.y, 
			camera->transform->position.y + cam_dim.y - offset.y), 
			camera->transform->position.y - cam_dim.y + offset.y);

		player2->setPos(player2_pos);

		// std::cout << cam_dim.x << " " << cam_dim.y << std::endl;
		// std::cout << next_cam_pos.x << " " << next_cam_pos.y << std::endl;
		//camera->transform->position.x =std::clamp(camera->transform->position.x, 140.0f, 300.0f);
		//camera->transform->position.y = std::clamp(camera->transform->position.y, 100.0f, 240.0f);
	}

	// Update lighting parameters
	PlayerStats::Instance().update(elapsed);

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

			// Reset collectables
			for (auto& c : collectableObjs)
			{
				c->setLife(1.0f);
			}

			std::cout << "reset triggered" << std::endl;
		}
	}

	
	//PlayerStats::Instance().to_string();
	{ //update listener to camera position:
		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 right = frame[0];
		glm::vec3 at = frame[3];
		Sound::listener.set_position_right(at, right, 1.0f / 60.0f);
	}	

	// UI update
	{
		glm::vec3 hb1pos = glm::vec3(glm::vec2(camera->transform->position), 0.f);
		hb1.setPos(hb1pos);
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);
	camera->drawable_size = drawable_size;

	GLCall(glClearColor(0.5f, 0.5f, 0.5f, 1.0f));
	GLCall(glClearDepth(1.0f)); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);

	auto draw_objects = [&]() {
		for (auto& obj : savePointObjs) {
			obj->draw(*camera);
		}
		for (auto& obj : moveableObjs) {
			obj->draw(*camera);
		}
		for (auto& obj : playerObjs) {
			obj->draw(*camera);
		}
		int n = (int)collectableObjs.size();
		for (int i = 0; i < n; i++) {
			//std::cout << i << std::endl;
			auto obj = collectableObjs[i];
			if (obj->getLife() > 0) {
				obj->draw(*camera);
			}
		}

		hb1->draw(*camera);
	};

	// first light
	{
		////set up light type and position for lit_color_texture_program:
		GLCall(glUseProgram(lit_color_texture_program->program));
		GLCall(glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 2));
		GLCall(glUniform3fv(lit_color_texture_program->LIGHT_LOCATION_vec3, 1, glm::value_ptr(glm::vec3(PlayerStats::Instance().player1Pos.x, PlayerStats::Instance().player1Pos.y, PlayerStats::Instance().player1LightDistance))));
		GLCall(glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, -1.0f))));
		GLCall(glUniform1f(lit_color_texture_program->LIGHT_CUTOFF_float, std::cos(3.1415926f * 0.8f)));
		GLCall(glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f) * 100.f * PlayerStats::Instance().player1LightEnergy)));
		GLCall(glUseProgram(0));
		scene.draw(*camera);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
	}

	// second light
	{
		//set up light type and position for lit_color_texture_program:
		GLCall(glUseProgram(lit_color_texture_program->program));
		GLCall(glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 2));
		GLCall(glUniform3fv(lit_color_texture_program->LIGHT_LOCATION_vec3, 1, glm::value_ptr(glm::vec3(PlayerStats::Instance().player2Pos.x, PlayerStats::Instance().player2Pos.y, PlayerStats::Instance().player2LightDistance))));
		GLCall(glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, -1.0f))));
		GLCall(glUniform1f(lit_color_texture_program->LIGHT_CUTOFF_float, std::cos(3.1415926f * 0.8f)));
		GLCall(glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f) * 100.f * PlayerStats::Instance().player2LightEnergy)));
		GLCall(glUseProgram(0));
		scene.draw(*camera);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
	}

	// third light
	{
		GLCall(glUseProgram(lit_color_texture_program->program));
		GLCall(glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 3));
		//GLCall(glUniform3fv(lit_color_texture_program->LIGHT_LOCATION_vec3, 1, glm::value_ptr(glm::vec3(PlayerStats::Instance().player1Pos.x, PlayerStats::Instance().player1Pos.y, 10.0f))));
		GLCall(glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, -1.0f))));
		GLCall(glUniform1f(lit_color_texture_program->LIGHT_CUTOFF_float, std::cos(3.1415926f * 0.8f)));
		GLCall(glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f) * 0.1f * PlayerStats::Instance().ambientLightEnergy)));
		GLCall(glUseProgram(0));
		scene.draw(*camera);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
	}

	// fourth light
	{
		GLCall(glDisable(GL_DEPTH_TEST));
		GLCall(glDepthMask(GL_FALSE));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		GLCall(glUseProgram(object_color_texture_program->program));
		GLCall(glUniform1i(object_color_texture_program->LIGHT_TYPE_int, 2));
		GLCall(glUniform3fv(object_color_texture_program->LIGHT_LOCATION_vec3, 1, glm::value_ptr(glm::vec3(PlayerStats::Instance().player1Pos.x, PlayerStats::Instance().player1Pos.y, PlayerStats::Instance().player1LightDistance))));
		GLCall(glUniform3fv(object_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, -1.0f))));
		GLCall(glUniform3fv(object_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f) * 400.0f * PlayerStats::Instance().player1LightEnergy)));
		GLCall(glUseProgram(0));

		draw_objects();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
	}

	// fifth light
	{
		GLCall(glDisable(GL_DEPTH_TEST));
		GLCall(glDepthMask(GL_FALSE));

		GLCall(glUseProgram(object_color_texture_program->program));
		GLCall(glUniform1i(object_color_texture_program->LIGHT_TYPE_int, 2));
		GLCall(glUniform3fv(object_color_texture_program->LIGHT_LOCATION_vec3, 1, glm::value_ptr(glm::vec3(PlayerStats::Instance().player2Pos.x, PlayerStats::Instance().player2Pos.y, PlayerStats::Instance().player2LightDistance))));
		GLCall(glUniform3fv(object_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, -1.0f))));
		GLCall(glUniform3fv(object_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f) * 400.0f * PlayerStats::Instance().player2LightEnergy)));
		GLCall(glUseProgram(0));

		draw_objects();
	}

	{
		GLCall(glDisable(GL_DEPTH_TEST));
		GLCall(glDepthMask(GL_FALSE));
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		for (auto& obj : markerObjs) obj->draw(*camera);
	}

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDisable(GL_BLEND));
	GLCall(glDepthMask(GL_TRUE));
	GL_ERRORS();
}