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
	glm::vec2 boxes1[4] = { glm::vec2(box1[0], box1[1]),
							glm::vec2(box1[0], box1[3]),
							glm::vec2(box1[2], box1[1]),
							glm::vec2(box1[2], box1[3])};

	glm::vec2 boxes2[4] = { glm::vec2(box2[0], box2[1]),
							glm::vec2(box2[0], box2[3]),
							glm::vec2(box2[2], box2[1]),
							glm::vec2(box2[2], box2[3])};
	for (size_t i = 0; i < 4; i++)
	{
		glm::vec2 p1 = boxes1[i];
		glm::vec2 p2 = boxes2[i];
		if (p1.x > box2[0] && p1.x < box2[2] &&
			p1.y > box2[1] && p1.y < box2[3])
			return true;
		if (p2.x > box1[0] && p2.x < box1[2] &&
			p2.y > box1[1] && p2.y < box1[3])
			return true;
	}
	return false;
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

	for (size_t i = 0; i < 4; i++)
	{
		sections.insert(static_cast<uint8_t>((player_points[i].x - GAME_MAP_ORIGIN.x) / unit_width) +
			static_cast<uint8_t>((player_points[i].y - GAME_MAP_ORIGIN.y) / unit_height) * COLLISION_OPT_LEVEL);
	}

	glm::vec4 player_box(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f,
		pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

	// Triggers
	for (const uint8_t i : sections)
	{
		const std::vector<std::shared_ptr<CollisionBox>>& triggers = scene_triggers[i];
		for (size_t j = 0; j < triggers.size(); j++)
		{
			bool res = IsCollided(player_box, triggers[j]->GetBoxCoord());
			if (res)
			{
				triggers[i]->OnTriggerEnter(player1_collision);
			}
		}
	}
}

void CollisionSystem::AddOneSceneBlock(const glm::vec2& pos, const glm::vec2& size, std::string name)
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
}

void CollisionSystem::AddOneThornBlock(const glm::vec2& pos, const glm::vec2& size, std::string name)
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
}