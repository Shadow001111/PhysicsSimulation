#pragma once
#include "RigidBody.h"
#include <vector>

class RigidPolygon : public RigidBody
{
	void updateTransformedVertices();
	void updateAABB() override;

	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> transformedVertices;
public:

	RigidPolygon(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, const Material& material, const std::vector<glm::vec2>& verts);

	const std::vector<glm::vec2>& getVertices();
	const std::vector<glm::vec2>& getTransformedVertices();
};

