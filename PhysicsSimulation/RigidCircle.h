#pragma once
#include <glm/glm.hpp>

class RigidCircle
{
public:
	glm::vec2 position;
	glm::vec2 velocity;
	float radius;

	RigidCircle(const glm::vec2& pos, const glm::vec2& vel, float r);
};

