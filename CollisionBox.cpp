#include "CollisionBox.hpp"

void CollisionBox::UpdateBoxCoord()
{
	box_coord[0] = pos.x - size.x * 0.5f;
	box_coord[1] = pos.y - size.y * 0.5f;
	box_coord[2] = pos.x + size.x * 0.5f;
	box_coord[3] = pos.y + size.y * 0.5f;
}

void ThornCollisionBox::OnTriggerEnter(std::shared_ptr<CollisionBox> cb)
{
	std::cout << "Somebody hits the throne" << std::endl;
	PlayerStats::Instance().health -= 100.f;	
}