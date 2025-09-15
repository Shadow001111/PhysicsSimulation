#include "RigidBody.h"

RigidBody::RigidBody(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, ShapeType shapeType) :
	position(pos), velocity(vel), rotation(rot), angularVelocity(angVel), mass(mass), inertia(inertia), material(material), shapeType(shapeType),
	aabb(), transformUpdateRequired(true), aabbUpdateRequired(true)
{
	invMass = mass == 0.0f ? 0.0f : 1.0f / mass;
	invInertia = inertia == 0.0f ? 0.0f : 1.0f / inertia;
}

bool RigidBody::isStatic() const
{
	return invMass == 0.0f;
}

const AABB& RigidBody::getAABB()
{
	if (aabbUpdateRequired)
	{
		updateAABB();
		aabbUpdateRequired = false;
	}
	return aabb;
}

Material::Material(float elasticity, float staticFriction, float dynamicFriction)
	: elasticity(elasticity), staticFriction(staticFriction), dynamicFriction(dynamicFriction)
{
}
