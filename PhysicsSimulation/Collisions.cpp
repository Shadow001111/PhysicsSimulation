#include "Collisions.h"
#include "math.h"
#include <iostream>

bool Collisions::circleCircle(RigidCircle& a, RigidCircle& b)
{
	glm::vec2 deltaPos = b.position - a.position;
	float distanceSquared = glm::dot(deltaPos, deltaPos);

	float radiusSum = a.radius + b.radius;
	if (distanceSquared >= radiusSum * radiusSum)
	{
		return false;
	}

	float distance = sqrtf(distanceSquared);
	glm::vec2 normal = deltaPos / distance;
	float displacement = radiusSum - distance;

	float bDisplacementRatio = a.mass / (a.mass + b.mass);
	float aDisplacementRatio = 1.0f - bDisplacementRatio;

	// Position
	a.position -= normal * (displacement * aDisplacementRatio);
	b.position += normal * (displacement * bDisplacementRatio);

	// Velocity
	float elasticity = 1.0f + fminf(a.elasticity, b.elasticity);

	glm::vec2 relVel = b.velocity - a.velocity;

	float velAlongNormal = glm::dot(relVel, normal);

	float j = elasticity * velAlongNormal / (a.invMass + b.invMass);

	a.velocity += (j * a.invMass) * normal;
	b.velocity -= (j * b.invMass) * normal;

	//
	a.transformUpdateRequired = true;
	b.transformUpdateRequired = true;
	return true;
}
