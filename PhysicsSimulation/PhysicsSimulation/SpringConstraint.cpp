#include "SpringConstraint.h"
#include <iostream>

SpringConstraint::SpringConstraint(RigidBody* bodyA, RigidBody* bodyB, const glm::vec2& anchorA, const glm::vec2& anchorB, float distance, float stiffness) :
	BaseConstraint(bodyA, bodyB, ConstraintType::Spring, anchorA, anchorB), distanceParam(distance), stiffnessParam(stiffness)
{
}

void SpringConstraint::update(float deltaTime)
{
	glm::vec2 posA = bodyA->position + rotatePoint(localAnchorA, bodyA->rotation);
	glm::vec2 posB = bodyB->position + rotatePoint(localAnchorB, bodyB->rotation);
	glm::vec2 dpos = posB - posA;

	float distance = glm::length(dpos);
	if (distance == 0.0f)
	{
		return;
	}

	glm::vec2 direction = dpos / distance;

	float difference = distanceParam - distance;
	float forceMagnitude = difference * stiffnessParam * deltaTime;
	glm::vec2 forceVector = direction * forceMagnitude;

	// TODO: Add methods for applying force in a certain point
	bodyA->applyImpulseAt(-forceVector, posA);
	bodyB->applyImpulseAt(forceVector, posB);
}