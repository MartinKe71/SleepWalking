#include "PlayerStats.hpp"

PlayerStats::PlayerStats()
{
	direction = 1;
	canJump = false;
	health = 100.f;
	rotMat = glm::mat3(1.f);
}

void PlayerStats::reset() 
{
	direction = 1;
	canJump = false;
	health = 100.f;
	rotMat = glm::mat3(1.f);
}