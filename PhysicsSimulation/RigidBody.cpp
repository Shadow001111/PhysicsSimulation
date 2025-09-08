#include "RigidBody.h"

RigidBody::RigidBody(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float elasticity, ShapeType shapeType) :
	position(pos), velocity(vel), rotation(rot), angularVelocity(angVel), mass(mass), elasticity(elasticity), shapeType(shapeType), transformUpdateRequired(true)
{
	invMass = mass == 0.0f ? 0.0f : 1.0f / mass;
}

void RigidBody::move(const glm::vec2& shift)
{
	position += shift;
	transformUpdateRequired = true;
}

void RigidBody::rotate(float angle)
{
	rotation += angle;
	transformUpdateRequired = true;
	// TODO: Check if angle is not zero
}

bool RigidBody::isStatic() const
{
	return invMass == 0.0f;
}

void RigidBody::trash() const
{
}
