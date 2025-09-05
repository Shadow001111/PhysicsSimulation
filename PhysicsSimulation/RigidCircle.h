#pragma once
#include <glm/glm.hpp>

class RigidCircle
{
public:
	glm::vec2 position;
	float radius;

	RigidCircle(const glm::vec2& pos, float r);
};

