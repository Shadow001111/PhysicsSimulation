#include "AxisConstraint.h"

AxisConstraint::AxisConstraint(RigidBody* bodyA, RigidBody* bodyB, const glm::vec2& anchorA, const glm::vec2& anchorB, bool disableX, bool disableY) :
	BaseConstraint(bodyA, bodyB, ConstraintType::Axis, anchorA, anchorB), disableX(disableX), disableY(disableY)
{
}

void AxisConstraint::update(float deltaTime)
{
	if (disableX)
	{
		bodyA->position.x = 0.0f;
		bodyA->velocity.x = 0.0f;
	}
	if (disableY)
	{
		bodyA->position.y = 0.0f;
		bodyA->velocity.y = 0.0f;
	}
}
