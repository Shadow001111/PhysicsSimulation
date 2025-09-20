#include "AxisConstraint.h"

AxisConstraint::AxisConstraint(RigidBody* body, bool disableX, bool disableY) :
	BaseConstraint(body, nullptr, ConstraintType::Axis, {}, {}), disableX(disableX), disableY(disableY), fixedPosition(body->position)
{
}

void AxisConstraint::update(float deltaTime)
{
	if (disableX)
	{
		bodyA->position.x = fixedPosition.x;
		bodyA->velocity.x = 0.0f;
	}
	if (disableY)
	{
		bodyA->position.y = fixedPosition.y;
		bodyA->velocity.y = 0.0f;
	}
}
