#include "BaseConstraint.h"

glm::vec2 BaseConstraint::rotatePoint(const glm::vec2& point, float angle)
{
	float cos_ = cosf(angle);
	float sin_ = sinf(angle);
	glm::vec2 rot =
	{
		point.x * cos_ - point.y * sin_,
		point.x * sin_ + point.y * cos_
	};
	return rot;
}

BaseConstraint::BaseConstraint(RigidBody* bodyA, RigidBody* bodyB, ConstraintType type, const glm::vec2& anchorA, const glm::vec2& anchorB) :
	bodyA(bodyA), bodyB(bodyB), type(type), localAnchorA(localAnchorA), localAnchorB(anchorB)
{
}
