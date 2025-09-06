#include "RigidCircle.h"

RigidCircle::RigidCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float radius, float mass, float elasticity)
	: RigidBody(pos, vel, rot, angVel, mass, elasticity, ShapeType::Circle), radius(radius)
{
}