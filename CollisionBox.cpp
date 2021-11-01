#include "CollisionBox.hpp"

CollisionBox::CollisionBox() {}

CollisionBox::~CollisionBox() {}

void PlayerCollisionBox::OnCollisionEnter()
{
	has_collided = true;
	std::cout << "Player has been hit." << std::endl;
}