#pragma once
#include <glm/glm.hpp>

enum class ShapeType : unsigned int
{
	Circle, Polygon
};

class RigidBody
{
public:
	glm::vec2 position, velocity;
	float rotation, angularVelocity;
	float mass, invMass;
	float elasticity;
	ShapeType shapeType;

	RigidBody(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float elasticity, ShapeType shapeType);

	bool isStatic() const;

	virtual void trash() const;
};

