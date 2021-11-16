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
	if (PlayerStats::Instance().player1Pos == glm::vec3(0.f))
		return;

	glm::vec2 player1s = glm::abs(PlayerStats::Instance().player1Size * glm::mat2(PlayerStats::Instance().rotMat));
	//glm::vec2 player1s = glm::abs(PlayerStats::Instance().player1Size);
	//glm::vec2 player2s = glm::abs(PlayerStats::Instance().player2Size);
	glm::vec3& player1p = PlayerStats::Instance().player1Pos;
	//glm::vec3& player2p = PlayerStats::Instance().player2Pos;
	std::vector<glm::vec2> player1_hitboxes_pos;
	std::vector<glm::vec2> player1_hitboxes_size;
	//glm::vec2 player2_hitbox_pos = glm::vec2(0.f);
	//glm::vec2 player2_hitbox_size = glm::vec2(0.f);

	if (PlayerCheckCollision_Stuck(player1p, player1s, player1_hitboxes_pos, player1_hitboxes_size))
	{
		std::cout << "Player 1 stuck in the wall." << std::endl;

		glm::vec4 player_box(player1p.x - player1s.x * 0.5f, player1p.y - player1s.y * 0.5f,
			player1p.x + player1s.x * 0.5f, player1p.y + player1s.y * 0.5f);

		size_t box_to_cal = 0;

		BoxCollisionType type = BoxCollisionType::NoInclude;

		for (size_t i = 0; i < player1_hitboxes_size.size(); i++)
		{
			const glm::vec2& box_pos = player1_hitboxes_pos[i];
			const glm::vec2& box_size = player1_hitboxes_size[i];
			
			glm::vec4 block_box(box_pos.x - box_size.x * 0.5f, box_pos.y - box_size.y * 0.5f,
				box_pos.x + box_size.x * 0.5f, box_pos.y + box_size.y * 0.5f);

			box_to_cal = i;
			type = CheckCollisionType(player_box, block_box);

			if (type == BoxCollisionType::XInclude || type == BoxCollisionType::YInclude)
				break;
		}

		float mindis_x = (player1s.x + player1_hitboxes_size[box_to_cal].x) * 0.5f;
		float mindis_y = (player1s.y + player1_hitboxes_size[box_to_cal].y) * 0.5f;

		float x_diff = std::max(0.0f, (mindis_x - std::abs(player1p.x - player1_hitboxes_pos[box_to_cal].x))) + 1.0f;
		float y_diff = std::max(0.0f, (mindis_y - std::abs(player1p.y - player1_hitboxes_pos[box_to_cal].y))) + 1.0f;

		glm::vec3 predict_pos = player1p;

		if (type == BoxCollisionType::XInclude)
		{
			int sign = (player1p.y - player1_hitboxes_pos[box_to_cal].y) > 0 ? 1 : -1;
			predict_pos = glm::vec3(player1p.x, player1p.y + sign * y_diff, player1p.z);
		}
		else if (type == BoxCollisionType::YInclude)
		{
			int sign = (player1p.x - player1_hitboxes_pos[box_to_cal].x) > 0 ? 1 : -1;
			predict_pos = glm::vec3(player1p.x + sign * x_diff, player1p.y, player1p.z);
		}
		else if (type == BoxCollisionType::NoInclude)
		{
			int sign_x = (player1p.x - player1_hitboxes_pos[box_to_cal].x) > 0 ? 1 : -1;
			int sign_y = (player1p.y - player1_hitboxes_pos[box_to_cal].y) > 0 ? 1 : -1;

			std::cout << "sign_x " << sign_x << ", sign_y " << sign_y << std::endl;

			if (x_diff > y_diff) {
				// Try y first
				predict_pos = glm::vec3(player1p.x, player1p.y + sign_y * y_diff, player1p.z);
				if (CheckPlayerBlockCollision(predict_pos, player1s, player1_hitboxes_pos[box_to_cal], player1_hitboxes_size[box_to_cal])
					|| predict_pos.y < 0)
				{
					predict_pos = glm::vec3(player1p.x + sign_x * x_diff, player1p.y, player1p.z);
					// Try x then
					if (CheckPlayerBlockCollision(predict_pos, player1s, player1_hitboxes_pos[box_to_cal], player1_hitboxes_size[box_to_cal])
						|| predict_pos.x < 0)
					{
						// Do both
						predict_pos = glm::vec3(player1p.x + sign_x * x_diff, player1p.y + sign_y * y_diff, player1p.z);
					}
				}
			}
			else
			{
				predict_pos = glm::vec3(player1p.x + sign_x * x_diff, player1p.y, player1p.z);
				if (CheckPlayerBlockCollision(predict_pos, player1s, player1_hitboxes_pos[box_to_cal], player1_hitboxes_size[box_to_cal])
					|| predict_pos.y < 0)
				{
					predict_pos = glm::vec3(player1p.x, player1p.y + sign_y * y_diff, player1p.z);
					// Try x then
					if (CheckPlayerBlockCollision(predict_pos, player1s, player1_hitboxes_pos[box_to_cal], player1_hitboxes_size[box_to_cal])
						|| predict_pos.x < 0)
					{
						// Do both
						predict_pos = glm::vec3(player1p.x + sign_x * x_diff, player1p.y + sign_y * y_diff, player1p.z);
					}
				}
			}
		}
		else if (type == BoxCollisionType::WholeInclude)
		{
			int sign_x = (player1p.x - player1_hitboxes_pos[box_to_cal].x) > 0 ? 1 : -1;
			int sign_y = (player1p.y - player1_hitboxes_pos[box_to_cal].y) > 0 ? 1 : -1;
			predict_pos = glm::vec3(player1p.x + sign_x * x_diff, player1p.y + sign_y * y_diff, player1p.z);
		}

		std::cout << "Collision type: " << type << std::endl;
		std::cout << "x_diff: " << x_diff << ", y_diff: " << y_diff << std::endl;
		std::cout << "Player Pos: " << player1p.x << ", " << player1p.y << ", " << player1p.z << std::endl;
		std::cout << "Predict Pos: " << predict_pos.x << ", " << predict_pos.y << ", " << predict_pos.z << std::endl;
		std::cout << "Box pos: " << player1_hitboxes_pos[box_to_cal].x << ", " << player1_hitboxes_pos[box_to_cal].y << std::endl;

		player1_collision->owner->setPos(predict_pos);
	}
}

bool CollisionSystem::IsCollided(const glm::vec4& box1, const glm::vec4& box2) const
{
	bool x_res = (box1[0] < box2[0] && box1[2] < box2[0]) || (box1[0] > box2[0] && box1[0] > box2[2]);
	bool y_res = (box1[1] < box2[1] && box1[3] < box2[1]) || (box1[1] > box2[1] && box1[1] > box2[3]);

	return !x_res && !y_res;
}

CollisionSystem::BoxCollisionType CollisionSystem::CheckCollisionType(const glm::vec4& box1, const glm::vec4& box2) const
{
	bool x_whole = !((box2[0] > box1[0] && box2[0] < box1[2]) || (box1[0] > box2[0] && box1[0] < box2[2]));
	bool y_whole = !((box2[1] > box1[1] && box2[1] < box1[3]) || (box1[1] > box2[1] && box1[1] < box2[3]));

	BoxCollisionType res = (x_whole && y_whole) ? BoxCollisionType::WholeInclude :
		(!x_whole && !y_whole) ? BoxCollisionType::NoInclude :
		(x_whole && !y_whole) ? BoxCollisionType::XInclude : 
		(!x_whole && y_whole) ? BoxCollisionType::YInclude : BoxCollisionType::WholeInclude;

	return res;
}

bool CollisionSystem::CheckPlayerBlockCollision(const glm::vec2& p1, const glm::vec2& s1, const glm::vec2& p2, const glm::vec2& s2)
{
	glm::vec4 player_box(p1.x - s1.x * 0.5f, p1.y - s1.y * 0.5f,
		p1.x + s1.x * 0.5f, p1.y + s1.y * 0.5f);
	glm::vec4 block_box(p2.x - s2.x * 0.5f, p2.y - s2.y * 0.5f,
		p2.x + s2.x * 0.5f, p2.y + s2.y * 0.5f);

	return IsCollided(player_box, block_box);
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
				/*std::cout << "Colldied block name: " << boxes[j]->name << std::endl;
				std::cout << "Collided block: 0: " << coord[0] << "; 1: " << coord[1] 
					<< ";2 : " << coord[2] << "; 3: " << coord[3] << std::endl;*/


				return true;
			}
		}
	}

	return false; 
}

bool CollisionSystem::PlayerCheckCollision_Stuck(const glm::vec2& pos, const glm::vec2& size, std::vector<glm::vec2>& boxes_pos, std::vector<glm::vec2>& boxes_size)
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

	bool ret_res = false;

	for (const uint8_t i : sections)
	{
		const std::vector<std::shared_ptr<BlockCollisionBox>>& boxes = scene_blocks[i];
		for (size_t j = 0; j < boxes.size(); j++)
		{
			bool res = IsCollided(player_box, boxes[j]->GetBoxCoord());
			if (res) {
				boxes_pos.push_back(boxes[j]->GetPos());
				boxes_size.push_back(boxes[j]->GetSize());
				ret_res = true;
				/*std::cout << "Colldied block name: " << boxes[j]->name << std::endl;
				std::cout << "Collided block: 0: " << coord[0] << "; 1: " << coord[1]
					<< ";2 : " << coord[2] << "; 3: " << coord[3] << std::endl;*/
			}
		}
	}

	return ret_res;
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

	//std::cout << "unit width : " << unit_width << " unit height : " << unit_height << std::endl;

	for (size_t i = 0; i < 4; i++)
	{
		sections.insert(static_cast<uint8_t>((player_points[i].x - GAME_MAP_ORIGIN.x) / unit_width) +
			static_cast<uint8_t>((player_points[i].y - GAME_MAP_ORIGIN.y) / unit_height) * COLLISION_OPT_LEVEL);
	}

	glm::vec4 player_box(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f,
		pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

	//std::cout << "iterating trigger arrays " << scene_triggers.size();


	std::shared_ptr<CollectableCollisionBox> to_delete;

	// Collectables
	for (const uint8_t i : sections)
	{
		//std::cout << " at index: " << std::to_string(i) << std::endl;
		std::vector<std::shared_ptr<CollectableCollisionBox>>& collectables = scenes_collectables[i];

		for (size_t j = 0; j < collectables.size(); j++)
		{
			bool res = IsCollided(player_box, collectables[j]->GetBoxCoord());
			if (res)
			{
				auto coord = collectables[j]->GetBoxCoord();
				/*std::cout << "Colldied block name: " << collectables[j]->name << std::endl;
				std::cout << "Collided block: 0: " << coord[0] << "; 1: " << coord[1]
					<< ";2 : " << coord[2] << "; 3: " << coord[3] << std::endl;*/

				collectables[j]->OnTriggerEnter(player1_collision);

				to_delete = collectables[j];

				//collectables.erase(collectables.begin() + j);

				//j--;
				break;
			}
		}
	}

	if (to_delete != nullptr) {
		//std::cout << "Erasing collectable collision box" << std::endl;
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