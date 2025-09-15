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

	RigidPolygon(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, const std::vector<glm::vec2>& verts);
	RigidPolygon(RigidPolygon&& other) noexcept;
	RigidPolygon& operator=(RigidPolygon&& other) noexcept;

	void move(const glm::vec2& shift) override;
	void rotate(float angle) override;
	void moveAndRotate(const glm::vec2& shift, float angle) override;

	const std::vector<glm::vec2>& getVertices();
	const std::vector<glm::vec2>& getTransformedVertices();
};

