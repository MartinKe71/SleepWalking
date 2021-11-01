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

	std::shared_ptr<CollisionBox> player_collision;


private:
	CollisionSystem() {}
};