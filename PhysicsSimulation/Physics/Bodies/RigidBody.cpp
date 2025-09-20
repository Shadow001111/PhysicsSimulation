#include "RigidBody.h"
#include "Core/CoreMath.h"

RigidBody::RigidBody(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, ShapeType shapeType) :
	position(pos), velocity(vel), rotation(rot), angularVelocity(angVel), mass(mass), inertia(inertia), localCenterOfMass(), material(material), shapeType(shapeType),
	aabb(), transformUpdateRequired(true), aabbUpdateRequired(true)
{
	invMass = mass == 0.0f ? 0.0f : 1.0f / mass;
	invInertia = inertia == 0.0f ? 0.0f : 1.0f / inertia;
}

void RigidBody::applyImpulseAt(const glm::vec2& impulse, const glm::vec2& point)
{
	// TODO: Cache center of mass
	glm::vec2 centerOfMass = getCenterOfMass();

	glm::vec2 relative = point - centerOfMass;

	velocity += impulse * invMass;
	angularVelocity += CoreMath::cross(relative, impulse) * invInertia;
}

bool RigidBody::isStatic() const
{
	return invMass == 0.0f;
}

glm::vec2 RigidBody::getCenterOfMass() const
{
	return position + localCenterOfMass;
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

void RigidBody::forceToUpdateAABB() const
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

	localCenterOfMass = properties.centerOfMass;
}


Material::Material(float elasticity, float staticFriction, float dynamicFriction)
	: elasticity(elasticity), staticFriction(staticFriction), dynamicFriction(dynamicFriction)
{
}
