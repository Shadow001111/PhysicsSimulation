#include "AngularVelocityConstraint.h"

AngularVelocityConstraint::AngularVelocityConstraint(RigidBody* body, float angularVelocity) :
	BaseConstraint(body, nullptr, ConstraintType::AngularVelocity, {}, {}), angularVelocity(angularVelocity)
{
}

void AngularVelocityConstraint::update(float deltaTime)
{
	bodyA->angularVelocity = angularVelocity;
}
