#pragma once
#include <glm/glm.hpp>

namespace CoreMath
{
	glm::vec2 rotatePoint(const glm::vec2& point, float angle);

	float cross(const glm::vec2& a, const glm::vec2& b);
}