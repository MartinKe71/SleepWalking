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

	std::shared_ptr<PlayerCollisionBox> player_collision;

	std::unordered_map<uint8_t, std::vector<std::shared_ptr<CollisionBox>>> scene_blocks;

	std::unordered_map<uint8_t, std::vector<std::shared_ptr<CollisionBox>>> scene_triggers;

	bool PlayerCheckCollision(const glm::vec2& pos, const glm::vec2& size);

private:
	CollisionSystem() {}
	bool IsCollided(const glm::vec4& box1, const glm::vec4& box2) const;
};