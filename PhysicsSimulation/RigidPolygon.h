#pragma once
#include "RigidBody.h"
#include <vector>

class RigidPolygon : public RigidBody
{
	void updateTransformedVertices();
public:
	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> transformedVertices;

	RigidPolygon(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float elasticity, const std::vector<glm::vec2>& verts);

	const std::vector<glm::vec2>& getVertices();
	const std::vector<glm::vec2>& getTransformedVertices();
};

