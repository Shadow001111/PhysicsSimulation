#include "RigidCircle.h"

#define _USE_MATH_DEFINES
#include <math.h>

void RigidCircle::updateAABB() const
{
	glm::vec2 dpos = glm::vec2(radius);
	aabb.min = position - dpos;
	aabb.max = position + dpos;
}

RigidCircle::RigidCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, float radius)
	: RigidBody(pos, vel, rot, angVel, mass, inertia, material, ShapeType::Circle), radius(radius)
{
}

void RigidCircle::move(const glm::vec2& shift)
{
	position += shift;
	aabb.min += shift;
	aabb.max += shift;
}

void RigidCircle::rotate(float angle)
{
	rotation += angle;
}

void RigidCircle::moveAndRotate(const glm::vec2& shift, float angle)
{
	position += shift;
	aabb.min += shift;
	aabb.max += shift;

	rotation += angle;
}

BodyProperties RigidCircle::calculateProperties(float density) const
{
	BodyProperties properties;

	float area = (float)M_PI * radius * radius;

	float mass = area * density;
	float inertia = 0.5f * mass * radius * radius;

	properties.mass = mass;
	properties.inertia = inertia;
	properties.centerOfMass = {};
	return properties;
}