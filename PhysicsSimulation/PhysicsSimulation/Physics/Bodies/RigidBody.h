#pragma once
#include "Core/AABB.h"

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

struct BodyProperties
{
	float mass;
	float inertia;
	//glm::vec2 centerOfMass;
	//float area;
};

class RigidBody
{
	virtual void updateAABB() const = 0;
public:
	glm::vec2 position, velocity;
	float rotation, angularVelocity;
	float mass, invMass;
	float inertia, invInertia;
	Material* material;
	ShapeType shapeType;
protected:
	mutable AABB aabb;
	mutable bool transformUpdateRequired;
	mutable bool aabbUpdateRequired;
public:
	RigidBody(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, ShapeType shapeType);

	virtual void move(const glm::vec2& shift) = 0;
	virtual void rotate(float angle) = 0;
	virtual void moveAndRotate(const glm::vec2& shift, float angle) = 0;

	void applyImpulseAt(const glm::vec2& impulse, const glm::vec2& point);

	bool isStatic() const;

	const AABB& getAABB() const;
	const AABB& getAABB_noUpdate() const;
	void forceToUpdateAABB();

	virtual BodyProperties calculateProperties(float density) const = 0;
	void setProperties(const BodyProperties& properties);
};