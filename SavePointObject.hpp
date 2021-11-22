#pragma once

#include "GameObject.hpp"
#include "CollisionBox.hpp"

class SavePointObject : public GameObject {
public:
	SavePointObject();
	SavePointObject(float mass = 10.0f, const glm::vec3& pos = glm::vec3(0.f),
		float w = 1.0f, float h = 1.f,
		const glm::vec3& vel = glm::vec3(0.f),
		bool isFixed = false,
		const std::string& filename = "", float l = 100.f);
	~SavePointObject();

	void reset() override;
	void createVerts() override;

	std::shared_ptr<>
};