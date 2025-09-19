#pragma once
#include "Physics/Bodies/RigidCircle.h"
#include "Physics/Bodies/RigidPolygon.h"

#include <memory>

struct CollisionManifold
{
	RigidBody* bodyA;
	RigidBody* bodyB;
	glm::vec2 normal;
	float depth = -1.0f;
	glm::vec2 contacts[2];
	unsigned int countOfContacts = 0;

	CollisionManifold() = default;
};

class Collisions
{
	static std::vector<CollisionManifold> manifolds;

	using CheckCollisionFunction = bool(*)(
		CollisionManifold&,
		const RigidBody*,
		const RigidBody*);

	static const CheckCollisionFunction checkCollisionFunctionsMatrix[2][2];

	static glm::vec2 projectVertices(const std::vector<glm::vec2>& vertices, glm::vec2 axis);
	static glm::vec2 projectCircle(const glm::vec2& position, float radius, glm::vec2 axis);
	static glm::vec2 findClosestVertexOnPolygon(const glm::vec2& point, const std::vector<glm::vec2>& vertices);
	static glm::vec2 findClosestPointOnSegment(const glm::vec2& start, const glm::vec2& end, const glm::vec2& point, float& outDistanceSquared);

	static bool circleCircle(CollisionManifold& result, const RigidBody* bodyA, const RigidBody* bodyB);
	static bool polygonPolygon(CollisionManifold& result, const RigidBody* bodyA, const RigidBody* bodyB);
	static bool circlePolygon(CollisionManifold& result, const RigidBody* bodyA, const RigidBody* bodyB);
public:
	static void checkCollision(std::unique_ptr<RigidBody>& bodyA, std::unique_ptr<RigidBody>& bodyB);
	static void checkCollision(RigidBody* bodyA, RigidBody* bodyB);

	static const std::vector<CollisionManifold>& getManifolds();
	static bool areAnyCollisionsFound();
	static void clearManifolds();
};

