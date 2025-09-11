#pragma once
#include "AABB.h"

enum class ShapeType : unsigned int
{
	Circle, Polygon
};

class RigidBody
{
	virtual void updateAABB() = 0;
public:
	glm::vec2 position, velocity;
	float rotation, angularVelocity;
	float mass, invMass;
	float inertia, invInertia;
	float elasticity;
	ShapeType shapeType;
protected:
	AABB aabb;
public:
	bool transformUpdateRequired;
	bool aabbUpdateRequired;

	RigidBody(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, float elasticity, ShapeType shapeType);

	void move(const glm::vec2& shift);
	void rotate(float angle);

	bool isStatic() const;

	const AABB& getAABB();
};

