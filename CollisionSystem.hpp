#pragma once

#include "CollisionBox.hpp"

class CollisionSystem {
public:

	CollisionSystem(CollisionSystem const&) = delete;
	void operator=(CollisionSystem const&) = delete;

	static CollisionSystem& Instance()
	{
		static CollisionSystem collision_system;
		return collision_system;
	}

	std::shared_ptr<PlayerCollisionBox> player1_collision;
	std::shared_ptr<PlayerCollisionBox> player2_collision;

	std::unordered_map<uint8_t, std::vector<std::shared_ptr<BlockCollisionBox>>> scene_blocks;

	std::unordered_map<uint8_t, std::vector<std::shared_ptr<CollisionBox>>> scene_triggers;

	std::unordered_map<uint8_t, std::vector<std::shared_ptr<CollectableCollisionBox>>> scenes_collectables;

	size_t unit_width = static_cast<size_t>(GAME_MAP_SIZE.x / COLLISION_OPT_LEVEL);
	size_t unit_height = static_cast<size_t>(GAME_MAP_SIZE.y / COLLISION_OPT_LEVEL);

	bool PlayerCheckCollision(const glm::vec2& pos, const glm::vec2& size);

	void PlayerCheckTrigger(const glm::vec2& pos, const glm::vec2& size);

	void PlayerCheckCollectables(const glm::vec2& pos, const glm::vec2& size);

	std::shared_ptr<BlockCollisionBox> AddOneSceneBlock(const glm::vec2& pos, const glm::vec2& size, std::string name);

	std::shared_ptr<CollisionBox> AddOneThornBlock(const glm::vec2& pos, const glm::vec2& size, std::string name);

	std::shared_ptr<CollectableCollisionBox> AddOneCollectable(const glm::vec2& pos, const glm::vec2& size, std::string name);

private:
	CollisionSystem();
	bool IsCollided(const glm::vec4& box1, const glm::vec4& box2) const;
};