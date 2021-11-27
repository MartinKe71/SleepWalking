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

	std::unordered_map<uint8_t, std::vector<std::shared_ptr<SavePointCollisionBox>>> scene_savePoints;

	size_t unit_width = static_cast<size_t>(GAME_MAP_SIZE.x / COLLISION_OPT_LEVEL);
	size_t unit_height = static_cast<size_t>(GAME_MAP_SIZE.y / COLLISION_OPT_LEVEL);

	bool PlayerCheckCollision(const glm::vec2& pos, const glm::vec2& size);

	void PlayerCheckTrigger(const glm::vec2& pos, const glm::vec2& size);

	void PlayerCheckCollectables(const glm::vec2& pos, const glm::vec2& size, bool isPlayer1 = true);

	std::shared_ptr<BlockCollisionBox> AddOneSceneBlock(const glm::vec2& pos, const glm::vec2& size, std::string name);

	std::shared_ptr<CollisionBox> AddOneThornBlock(const glm::vec2& pos, const glm::vec2& size, std::string name);

	std::shared_ptr<CollectableCollisionBox> AddOneCollectable(const glm::vec2& pos, const glm::vec2& size, std::string name);

	std::shared_ptr<SavePointCollisionBox> AddOneSavePoint(const glm::vec2& pos, const glm::vec2& size, std::string name);

	void update(float elapsed);

private:
	enum BoxCollisionType {
		// One axis intersect, one axis include
		XInclude, 
		
		YInclude,

		// Two axis include,
		WholeInclude,

		// Two axis intersect,
		NoInclude
	};

	CollisionSystem();
	bool IsCollided(const glm::vec4& box1, const glm::vec4& box2) const;
	bool PlayerCheckCollision_Stuck(const glm::vec2& pos, const glm::vec2& size, std::vector<glm::vec2>& boxes_pos, std::vector<glm::vec2>& boxes_size);
	bool CheckPlayerBlockCollision(const glm::vec2& p1, const glm::vec2& s1, const glm::vec2& p2, const glm::vec2& s2);
	BoxCollisionType CheckCollisionType(const glm::vec4& box1, const glm::vec4& box2) const;
};