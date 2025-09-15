#pragma once
#include "RigidBody.h"

class RigidCircle : public RigidBody
{
	void updateAABB() override;
public:
	float radius;

	RigidCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, const Material& material, float radius);

	void move(const glm::vec2& shift) override;
	void rotate(float angle) override;
	void moveAndRotate(const glm::vec2& shift, float angle) override;
};

