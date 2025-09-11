#include "RigidCircle.h"

void RigidCircle::updateAABB()
{
	glm::vec2 dpos = glm::vec2(radius);
	aabb.min = position - dpos;
	aabb.max = position + dpos;
}

RigidCircle::RigidCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, float elasticity, float radius)
	: RigidBody(pos, vel, rot, angVel, mass, inertia, elasticity, ShapeType::Circle), radius(radius)
{
}