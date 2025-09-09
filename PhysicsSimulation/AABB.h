#pragma once
#include "glm/glm.hpp"

struct AABB
{
	glm::vec2 min, max;

	AABB();
	AABB(const glm::vec2& min, const glm::vec2& max);
	AABB(float minX, float minY, float maxX, float maxY);
};

