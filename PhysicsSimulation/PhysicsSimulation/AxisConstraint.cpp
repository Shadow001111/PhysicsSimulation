#include "AxisConstraint.h"

AxisConstraint::AxisConstraint(RigidBody* body, bool disableX, bool disableY) :
	BaseConstraint(body, nullptr, ConstraintType::Axis, {}, {}), disableX(disableX), disableY(disableY)
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
