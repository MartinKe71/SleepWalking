#include "Inivar.hpp"
#include "PlayerStats.hpp"
#include <iostream>
#include <glm/gtx/string_cast.hpp> 

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
	jumpElapsed = 0.f;

	player1Light = std::max(player1SavedLight, 4.0f);
	player2Light = std::max(player2SavedLight, 4.0f);

	std::cout << "Player 1 Light: " << player1Light << std::endl;
	std::cout << "Player 2 Light: " << player2Light << std::endl;
}

void PlayerStats::update(float elapsed)
{	
	player1Light = std::max(player1Light - LIGHT_DECAY * elapsed, 0.f);
	player2Light = std::max(player2Light - LIGHT_DECAY * elapsed, 0.f);
	
	player1LightEnergy = player1Light / 5.0f;
	player2LightEnergy = player2Light / 5.0f;

	ambientLightEnergy = std::min((player1LightEnergy + player2LightEnergy) / 2.f, 1.0f);
	
	if (player1Light == 0.f && player2Light == 0.f) health = 0.f;
}

void PlayerStats::to_string() {
	std::cout << "\n\n-----------------------\n";
	std::cout << "Printing Player Stats" << std::endl;
	std::cout << "Player Health: " << health << std::endl;
	std::cout << "Player Light Num: " << lightNum << std::endl;
	std::cout << "Player 1 Position: " << glm::to_string(player1Pos) << std::endl;
	std::cout << "Player 2 Position: " << glm::to_string(player2Pos) << std::endl;
	std::cout << "-----------------------\n\n\n";
}