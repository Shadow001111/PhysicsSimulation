#include "RigidBody.h"

RigidBody::RigidBody(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, ShapeType shapeType) :
	position(pos), velocity(vel), rotation(rot), angularVelocity(angVel), mass(mass), inertia(inertia), material(material), shapeType(shapeType),
	aabb(), transformUpdateRequired(true), aabbUpdateRequired(true)
{
	invMass = mass == 0.0f ? 0.0f : 1.0f / mass;
	invInertia = inertia == 0.0f ? 0.0f : 1.0f / inertia;
}

void RigidBody::applyImpulseAt(const glm::vec2& impulse, const glm::vec2& point)
{
	glm::vec2 relative = point - position;
	glm::vec2 perp = { -relative.y, relative.x };

	velocity += impulse * invMass;
	angularVelocity += glm::dot(perp, impulse) * invInertia;
}

bool RigidBody::isStatic() const
{
	return invMass == 0.0f;
}

const AABB& RigidBody::getAABB() const
{
	if (aabbUpdateRequired)
	{
		updateAABB();
		aabbUpdateRequired = false;
	}
	return aabb;
}

const AABB& RigidBody::getAABB_noUpdate() const
{
	return aabb;
}

void RigidBody::forceToUpdateAABB()
{
	if (aabbUpdateRequired)
	{
		updateAABB();
		aabbUpdateRequired = false;
	}
}

void RigidBody::setProperties(const BodyProperties& properties)
{
	mass = properties.mass;
	invMass = properties.mass == 0.0f ? 0.0f : 1.0f / properties.mass;

	inertia = properties.inertia;
	invInertia = properties.inertia == 0.0f ? 0.0f : 1.0f / properties.inertia;
}


Material::Material(float elasticity, float staticFriction, float dynamicFriction)
	: elasticity(elasticity), staticFriction(staticFriction), dynamicFriction(dynamicFriction)
{
}
