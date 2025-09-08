#pragma once
#include "RigidBody.h"

class RigidCircle : public RigidBody
{
public:
	float radius;

	RigidCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float elasticity, float radius);
};

