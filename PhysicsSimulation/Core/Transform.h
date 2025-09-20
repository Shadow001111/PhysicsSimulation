#pragma once
#include "glm/glm.hpp"

class Transform
{
public:
	glm::vec2 position;
	glm::vec2 cos_sin;

	Transform(glm::vec2 pos, float angle);

	glm::vec2 transform(const glm::vec2& in) const;
};

