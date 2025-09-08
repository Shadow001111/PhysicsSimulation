#pragma once
#include "RigidCircle.h"
#include "RigidPolygon.h"

#include <memory>

class Collisions
{
	static glm::vec2 projectVertices(const std::vector<glm::vec2>& vertices, glm::vec2 axis);
public:
	static bool circleCircle(RigidCircle& a, RigidCircle& b);
	static bool circleCircle(std::unique_ptr<RigidBody>& a, std::unique_ptr<RigidBody>& b);

	static bool polygonPolygon(RigidPolygon& a, RigidPolygon& b);
	static bool polygonPolygon(std::unique_ptr<RigidBody>& a, std::unique_ptr<RigidBody>& b);
};

