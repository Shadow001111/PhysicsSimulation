#include "RigidCircle.h"

void RigidCircle::updateAABB()
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
