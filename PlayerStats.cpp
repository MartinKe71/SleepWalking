#include "PlayerStats.hpp"

PlayerStats::PlayerStats()
{
	direction = 1;
	canJump = false;
	health = 100.f;
}

void PlayerStats::reset() 
{
	direction = 1;
	canJump = false;
	health = 100.f;
}