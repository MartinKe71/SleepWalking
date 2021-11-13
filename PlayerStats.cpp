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