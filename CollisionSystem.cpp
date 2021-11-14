#include "CollisionSystem.hpp"

CollisionSystem::CollisionSystem()
{
	for (uint8_t i = 0; i < COLLISION_OPT_LEVEL * COLLISION_OPT_LEVEL; i++)
	{
		std::vector<std::shared_ptr<BlockCollisionBox>> v;
		scene_blocks[i] = v;
		std::vector<std::shared_ptr<CollisionBox>> v_1;
		scene_triggers[i] = v_1;
	}
}

void CollisionSystem::update(float elapsed)
{
	glm::vec2 player1s = glm::abs(PlayerStats::Instance().player1Size * glm::mat2(PlayerStats::Instance().rotMat));
	//glm::vec2 player1s = glm::abs(PlayerStats::Instance().player1Size);
	glm::vec2 player2s = glm::abs(PlayerStats::Instance().player2Size);
	glm::vec3& player1p = PlayerStats::Instance().player1Pos;
	glm::vec3& player2p = PlayerStats::Instance().player2Pos;
	glm::vec2 player1_hitbox_pos = glm::vec2(0.f);
	glm::vec2 player1_hitbox_size = glm::vec2(0.f);
	glm::vec2 player2_hitbox_pos = glm::vec2(0.f);
	glm::vec2 player2_hitbox_size = glm::vec2(0.f);

	if (PlayerCheckCollision_Stuck(PlayerStats::Instance().player1Pos, player1s, player1_hitbox_pos, player1_hitbox_size))
	{
		//std::cout << "Player 1 stuck in the wall." << std::endl;
		//std::cout << "Collided block: 0: " << player1_hit_box[0] << "; 1: " << player1_hit_box[1]
		//	<< ";2 : " << player1_hit_box[2] << "; 3: " << player1_hit_box[3] << std::endl;

		//float x_diff = (player1_hitbox_pos.x > player1p.x) ? 1.0f : -1.0f;
		//float y_diff = (player1_hitbox_pos.y > player1p.y) ? 1.0f : -1.0f;

		float x_mindis = (player1s.x + player1_hitbox_size.x) * 0.5f;
		float y_mindis = (player1s.y + player1_hitbox_size.y) * 0.5f;

		float x_diff = std::max(0.0f,(x_mindis - std::abs(player1p.x - player1_hitbox_pos.x)));
		float y_diff = std::max(0.0f,(y_mindis - std::abs(player1p.y - player1_hitbox_pos.y)));

		std::cout << "Hit the wall " << std::endl;

		x_diff *= (player1_hitbox_pos.x > player1p.x) ? -1.0f : 1.0f;
		y_diff *= (player1_hitbox_pos.y > player1p.y) ? -1.0f : 1.0f;

		std::cout << "X DIFF: " << x_diff << std::endl;
		std::cout << "Y DIFF: " << y_diff << std::endl;
		
		glm::vec3 resPos_x = glm::vec3(player1p.x + x_diff, player1p.y, player1p.z);
		glm::vec3 resPos_y = glm::vec3(player1p.x, player1p.y + y_diff, player1p.z);
		glm::vec3 resPos_xy = glm::vec3(player1p.x + x_diff, player1p.y + y_diff, player1p.z);

		if (PlayerCheckCollision(resPos_x, player1s))
		{
			if (PlayerCheckCollision(resPos_y, player1s))
				player1_collision->owner->setPos(resPos_xy);
			else
				player1_collision->owner->setPos(resPos_y);
		}
		else
			player1_collision->owner->setPos(resPos_x);
	}

	if (PlayerCheckCollision_Stuck(PlayerStats::Instance().player2Pos, player2s, player2_hitbox_pos, player2_hitbox_size))
	{
		//std::cout << "Player 2 stuck in the wall." << std::endl;
	}
}


bool CollisionSystem::IsCollided(const glm::vec4& box1, const glm::vec4& box2) const 
{
	bool x_res = (box1[0] < box2[0] && box1[2] < box2[0]) || (box1[0] > box2[0] && box1[0] > box2[2]);
	bool y_res = (box1[1] < box2[1] && box1[3] < box2[1]) || (box1[1] > box2[1] && box1[1] > box2[3]);

	return !x_res && !y_res;
}

bool CollisionSystem::PlayerCheckCollision(const glm::vec2& pos, const glm::vec2& size)
{
	glm::vec2 player_points[4] = { glm::vec2(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f),
									glm::vec2(pos.x - size.x * 0.5f, pos.y + size.y * 0.5f),
									glm::vec2(pos.x + size.x * 0.5f, pos.y - size.y * 0.5f),
									glm::vec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f)};

	std::set<uint8_t> sections;

	for (size_t i = 0; i < 4; i++)
	{
		sections.insert (static_cast<uint8_t>((player_points[i].x - GAME_MAP_ORIGIN.x) / unit_width) + 
			static_cast<uint8_t>((player_points[i].y - GAME_MAP_ORIGIN.y) / unit_height) * COLLISION_OPT_LEVEL);
	}

	glm::vec4 player_box(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f,
		pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

	for (const uint8_t i : sections)
	{
		const std::vector<std::shared_ptr<BlockCollisionBox>> & boxes = scene_blocks[i];
		for (size_t j = 0; j < boxes.size(); j++)
		{
			bool res = IsCollided(player_box, boxes[j]->GetBoxCoord());
			if (res) { 
				auto coord = boxes[j]->GetBoxCoord();
				std::cout << "Colldied block name: " << boxes[j]->name << std::endl;
				std::cout << "Collided block: 0: " << coord[0] << "; 1: " << coord[1] 
					<< ";2 : " << coord[2] << "; 3: " << coord[3] << std::endl;


				return true;
			}
		}
	}

	return false; 
}

bool CollisionSystem::PlayerCheckCollision_Stuck(const glm::vec2& pos, const glm::vec2& size, glm::vec2& box_pos, glm::vec2& box_size)
{
	glm::vec2 player_points[4] = { glm::vec2(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f),
									glm::vec2(pos.x - size.x * 0.5f, pos.y + size.y * 0.5f),
									glm::vec2(pos.x + size.x * 0.5f, pos.y - size.y * 0.5f),
									glm::vec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f) };

	std::set<uint8_t> sections;

	for (size_t i = 0; i < 4; i++)
	{
		sections.insert(static_cast<uint8_t>((player_points[i].x - GAME_MAP_ORIGIN.x) / unit_width) +
			static_cast<uint8_t>((player_points[i].y - GAME_MAP_ORIGIN.y) / unit_height) * COLLISION_OPT_LEVEL);
	}

	glm::vec4 player_box(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f,
		pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

	for (const uint8_t i : sections)
	{
		const std::vector<std::shared_ptr<BlockCollisionBox>>& boxes = scene_blocks[i];
		for (size_t j = 0; j < boxes.size(); j++)
		{
			bool res = IsCollided(player_box, boxes[j]->GetBoxCoord());
			if (res) {
				box_pos = boxes[j]->GetPos();
				box_size = boxes[j]->GetSize();
				/*std::cout << "Colldied block name: " << boxes[j]->name << std::endl;
				std::cout << "Collided block: 0: " << coord[0] << "; 1: " << coord[1]
					<< ";2 : " << coord[2] << "; 3: " << coord[3] << std::endl;*/
				return true;
			}
		}
	}

	return false;
}


void CollisionSystem::PlayerCheckTrigger(const glm::vec2& pos, const glm::vec2& size)
{
	glm::vec2 player_points[4] = { glm::vec2(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f),
								glm::vec2(pos.x - size.x * 0.5f, pos.y + size.y * 0.5f),
								glm::vec2(pos.x + size.x * 0.5f, pos.y - size.y * 0.5f),
								glm::vec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f) };

	std::set<uint8_t> sections;

	//std::cout << "unit width : " << unit_width << " unit height : " << unit_height << std::endl;

	for (size_t i = 0; i < 4; i++)
	{
		sections.insert(static_cast<uint8_t>((player_points[i].x - GAME_MAP_ORIGIN.x) / unit_width) +
			static_cast<uint8_t>((player_points[i].y - GAME_MAP_ORIGIN.y) / unit_height) * COLLISION_OPT_LEVEL);
	}

	glm::vec4 player_box(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f,
		pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

	//std::cout << "iterating trigger arrays " << scene_triggers.size();

	// Triggers
	for (const uint8_t i : sections)
	{
		//std::cout << " at index: " << std::to_string(i) << std::endl;
		const std::vector<std::shared_ptr<CollisionBox>>& triggers = scene_triggers[i];
		for (size_t j = 0; j < triggers.size(); j++)
		{
			bool res = IsCollided(player_box, triggers[j]->GetBoxCoord());
			if (res)
			{
				triggers[i]->OnTriggerEnter(player1_collision);
				return;
			}
		}
	}
}

void CollisionSystem::PlayerCheckCollectables(const glm::vec2& pos, const glm::vec2& size)
{
	glm::vec2 player_points[4] = { glm::vec2(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f),
								glm::vec2(pos.x - size.x * 0.5f, pos.y + size.y * 0.5f),
								glm::vec2(pos.x + size.x * 0.5f, pos.y - size.y * 0.5f),
								glm::vec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f) };

	std::set<uint8_t> sections;

	std::cout << "unit width : " << unit_width << " unit height : " << unit_height << std::endl;

	for (size_t i = 0; i < 4; i++)
	{
		sections.insert(static_cast<uint8_t>((player_points[i].x - GAME_MAP_ORIGIN.x) / unit_width) +
			static_cast<uint8_t>((player_points[i].y - GAME_MAP_ORIGIN.y) / unit_height) * COLLISION_OPT_LEVEL);
	}

	glm::vec4 player_box(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f,
		pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

	std::cout << "iterating trigger arrays " << scene_triggers.size();


	std::shared_ptr<CollectableCollisionBox> to_delete;

	// Collectables
	for (const uint8_t i : sections)
	{
		std::cout << " at index: " << std::to_string(i) << std::endl;
		std::vector<std::shared_ptr<CollectableCollisionBox>>& collectables = scenes_collectables[i];

		for (size_t j = 0; j < collectables.size(); j++)
		{
			bool res = IsCollided(player_box, collectables[j]->GetBoxCoord());
			if (res)
			{
				auto coord = collectables[j]->GetBoxCoord();
				std::cout << "Colldied block name: " << collectables[j]->name << std::endl;
				std::cout << "Collided block: 0: " << coord[0] << "; 1: " << coord[1]
					<< ";2 : " << coord[2] << "; 3: " << coord[3] << std::endl;

				collectables[j]->OnTriggerEnter(player1_collision);

				to_delete = collectables[j];

				//collectables.erase(collectables.begin() + j);

				//j--;
				break;
			}
		}
	}

	if (to_delete != nullptr) {
		std::cout << "Erasing collectable collision box" << std::endl;
		for (uint8_t i : sections) {
			auto pos = find(scenes_collectables[i].begin(), scenes_collectables[i].end(), to_delete);
			scenes_collectables[i].erase(pos);
		}
	}
}

std::shared_ptr<BlockCollisionBox> CollisionSystem::AddOneSceneBlock(const glm::vec2& pos, const glm::vec2& size, std::string name)
{
	glm::vec2 block_points[4] = { glm::vec2(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f),
									glm::vec2(pos.x - size.x * 0.5f, pos.y + size.y * 0.5f),
									glm::vec2(pos.x + size.x * 0.5f, pos.y - size.y * 0.5f),
									glm::vec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f) };

	std::set<uint8_t> sections;

	for (size_t i = 0; i < 4; i++)
	{
		sections.insert(static_cast<uint8_t>((block_points[i].x - GAME_MAP_ORIGIN.x) / unit_width) +
			static_cast<uint8_t>((block_points[i].y - GAME_MAP_ORIGIN.y) / unit_height) * COLLISION_OPT_LEVEL);
	}

	std::shared_ptr<BlockCollisionBox> box(new BlockCollisionBox(pos, size, false, name));
	
	for (const uint8_t i : sections)
	{
		scene_blocks[i].push_back(box);
	}

	return box;
}

std::shared_ptr<CollisionBox> CollisionSystem::AddOneThornBlock(const glm::vec2& pos, const glm::vec2& size, std::string name)
{
	glm::vec2 block_points[4] = { glm::vec2(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f),
								glm::vec2(pos.x - size.x * 0.5f, pos.y + size.y * 0.5f),
								glm::vec2(pos.x + size.x * 0.5f, pos.y - size.y * 0.5f),
								glm::vec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f) };

	std::set<uint8_t> sections;

	for (size_t i = 0; i < 4; i++)
	{
		sections.insert(static_cast<uint8_t>((block_points[i].x - GAME_MAP_ORIGIN.x) / unit_width) +
			static_cast<uint8_t>((block_points[i].y - GAME_MAP_ORIGIN.y) / unit_height) * COLLISION_OPT_LEVEL);
	}

	std::shared_ptr<CollisionBox> box(new ThornCollisionBox(pos, size, true, name));

	for (const uint8_t i : sections)
	{
		scene_triggers[i].push_back(box);
	}

	return box;
}

std::shared_ptr<CollectableCollisionBox> CollisionSystem::AddOneCollectable(const glm::vec2& pos, const glm::vec2& size, std::string name)
{
	glm::vec2 block_points[4] = { glm::vec2(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f),
								glm::vec2(pos.x - size.x * 0.5f, pos.y + size.y * 0.5f),
								glm::vec2(pos.x + size.x * 0.5f, pos.y - size.y * 0.5f),
								glm::vec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f) };

	std::set<uint8_t> sections;

	for (size_t i = 0; i < 4; i++)
	{
		sections.insert(static_cast<uint8_t>((block_points[i].x - GAME_MAP_ORIGIN.x) / unit_width) +
			static_cast<uint8_t>((block_points[i].y - GAME_MAP_ORIGIN.y) / unit_height) * COLLISION_OPT_LEVEL);
	}

	std::shared_ptr<CollectableCollisionBox> box(new CollectableCollisionBox(pos, size, true, name));

	for (const uint8_t i : sections)
	{
		scenes_collectables[i].push_back(box);
	}

	return box;
}