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
	Material* material;
	ShapeType shapeType;
protected:
	AABB aabb;
	bool transformUpdateRequired;
	bool aabbUpdateRequired;
public:

	RigidBody(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, ShapeType shapeType);

	virtual void move(const glm::vec2& shift) = 0;
	virtual void rotate(float angle) = 0;
	virtual void moveAndRotate(const glm::vec2& shift, float angle) = 0;

	bool isStatic() const;

	const AABB& getAABB();
};

