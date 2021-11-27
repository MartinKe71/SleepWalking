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
	//std::cout << "Somebody hits the thorn" << std::endl;
	PlayerStats::Instance().health -= 100.f;	
}

void CollectableCollisionBox::OnTriggerEnter(std::shared_ptr<CollisionBox> cb)
{
	std::cout << "Somebody hits the collectable" << cb->name << std::endl;
	if (cb->name == "player1") {
		PlayerStats::Instance().player1Light += 1.0f;
	}
	else if (cb->name == "player2") {
		PlayerStats::Instance().player2Light += 1.0f;
	}
	else {
		std::cerr << "Non-player object is triggering collectables" << std::endl;
	}

	//PlayerStats::Instance().lightNum++;

	if (owner != nullptr) {
		owner->setLife(-1.f);
	}
}

void SavePointCollisionBox::OnTriggerEnter(std::shared_ptr<CollisionBox> cb)
{
	//std::cout << "Save point hit: " << name << std::endl;

	if (PlayerStats::Instance().player1SavedPos.x != owner->getPos().x ||
		PlayerStats::Instance().player1SavedPos.y != owner->getPos().y)
	{
		PlayerStats::Instance().player1SavedPos.x = owner->getPos().x;
		PlayerStats::Instance().player1SavedPos.y = owner->getPos().y;

		PlayerStats::Instance().player2SavedPos.x = owner->getPos().x - 5.0f;
		PlayerStats::Instance().player2SavedPos.y = owner->getPos().y + 5.f;

		PlayerStats::Instance().player1SavedLight = PlayerStats::Instance().player1Light;
		PlayerStats::Instance().player2SavedLight = PlayerStats::Instance().player2Light;
	}
	//std::cout << "Save point hit: " << name << ", x: " << owner->getPos().x
	//										<< ", y: " << owner->getPos().y 
	//										<< ", z: " << owner->getPos().z << std::endl;
}