#include "Transform.h"
#include "math.h"

Transform::Transform(glm::vec2 pos, float angle) :
	position(pos), cos_sin(cosf(angle), sinf(angle))
{
}

glm::vec2 Transform::transform(const glm::vec2& in) const
{
	glm::vec2 rot =
	{
		in.x * cos_sin.x - in.y * cos_sin.y,
		in.x * cos_sin.y + in.y * cos_sin.x
	};

	glm::vec2 trans = rot + position;

	return trans;
}
