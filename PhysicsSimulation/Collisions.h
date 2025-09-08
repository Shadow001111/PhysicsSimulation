#pragma once
#include "RigidCircle.h"
#include "RigidPolygon.h"

#include <memory>

struct CollisionInfo
{
	glm::vec2 normal;
	float depth = -1.0f;
};

class Collisions
{
	static glm::vec2 projectVertices(const std::vector<glm::vec2>& vertices, glm::vec2 axis);
public:
	static CollisionInfo circleCircle(RigidCircle& a, RigidCircle& b);
	static CollisionInfo circleCircle(std::unique_ptr<RigidBody>& a, std::unique_ptr<RigidBody>& b);

	static CollisionInfo polygonPolygon(RigidPolygon& a, RigidPolygon& b);
	static CollisionInfo polygonPolygon(std::unique_ptr<RigidBody>& a, std::unique_ptr<RigidBody>& b);
};

