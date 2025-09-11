#pragma once
#include "AABB.h"

enum class ShapeType : unsigned int
{
	Circle, Polygon
};

struct Material
{
	float elasticity = 1.0f;
	float staticFriction = 0.0f, dynamicFriction = 0.0f;

	Material() = default;
	Material(float elasticity, float staticFriction, float dynamicFriction);
};

class RigidBody
{
	virtual void updateAABB() = 0;
public:
	glm::vec2 position, velocity;
	float rotation, angularVelocity;
	float mass, invMass;
	float inertia, invInertia;
	Material material;
	ShapeType shapeType;
protected:
	AABB aabb;
public:
	bool transformUpdateRequired;
	bool aabbUpdateRequired;

	RigidBody(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, const Material& material, ShapeType shapeType);

	void move(const glm::vec2& shift);
	void rotate(float angle);

	bool isStatic() const;

	const AABB& getAABB();
};

