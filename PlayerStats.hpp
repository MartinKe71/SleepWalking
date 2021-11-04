#pragma once

#include <glm/glm.hpp>

class PlayerStats {
public:

	PlayerStats(PlayerStats const&) = delete;
	void operator=(PlayerStats const&) = delete;

	static PlayerStats& Instance()
	{
		static PlayerStats playerStat;
		return playerStat;
	}

	float health = 100.f;
	int direction = 1;
	bool canJump = true;
	glm::vec3 startPos = glm::vec3(0.f);
	glm::vec3 startVel = glm::vec3(0.f);

	void reset();

private:
	PlayerStats();
};