#include "RigidCircle.h"

RigidCircle::RigidCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float elasticity, float radius)
	: RigidBody(pos, vel, rot, angVel, mass, elasticity, ShapeType::Circle), radius(radius)
{
}