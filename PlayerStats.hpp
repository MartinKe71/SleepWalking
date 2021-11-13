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
	int lightNum = 0;
	int direction = 1;
	
	glm::mat3 rotMat = glm::mat3(1.f);
	bool isFacingLeft = false;

	bool canJump = true;

	glm::vec3 player1Pos = glm::vec3(0.f);
	glm::vec3 player2Pos = glm::vec3(0.f);

	glm::vec3 player1SavedPos = glm::vec3(0.f);
	glm::vec3 player1SavedVel = glm::vec3(0.f);
	glm::vec3 player2SavedPos = glm::vec3(0.f);
	glm::vec3 player2SavedVel = glm::vec3(0.f);
	float jumpElapsed = 0.f;

	void reset();

	void to_string();

private:
	PlayerStats();
};