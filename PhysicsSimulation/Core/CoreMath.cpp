#include "CoreMath.h"
#include <math.h>

namespace CoreMath
{
	glm::vec2 rotatePoint(const glm::vec2& point, float angle)
	{
		float cos_ = cosf(angle);
		float sin_ = sinf(angle);

		return
		{
			point.x * cos_ - point.y * sin_,
			point.x * sin_ + point.y * cos_
		};
	}

	float cross(const glm::vec2& a, const glm::vec2& b)
	{
		return a.x * b.y - a.y * b.x;
	}
}