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