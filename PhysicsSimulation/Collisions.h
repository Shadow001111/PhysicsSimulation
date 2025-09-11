#pragma once
#include "RigidCircle.h"
#include "RigidPolygon.h"

#include <memory>

struct CollisionManifold
{
	std::unique_ptr<RigidBody>* bodyA;
	std::unique_ptr<RigidBody>* bodyB;
	glm::vec2 normal;
	float depth = -1.0f;
	glm::vec2 contacts[2];
	int countOfContacts = 0;

	CollisionManifold() = default;
	CollisionManifold(std::unique_ptr<RigidBody>* a, std::unique_ptr<RigidBody>* b, const glm::vec2& n, float d, const glm::vec2& contact1, const glm::vec2& contact2, int countOfContacts);
};

class Collisions
{
	static std::vector<CollisionManifold> manifolds;

	static glm::vec2 projectVertices(const std::vector<glm::vec2>& vertices, glm::vec2 axis);
	static glm::vec2 projectCircle(const glm::vec2& position, float radius, glm::vec2 axis);
	static glm::vec2 findClosestPointOnPolygon(const glm::vec2& point, const std::vector<glm::vec2>& vertices);

	static void circleCircle(RigidCircle& a, RigidCircle& b, std::unique_ptr<RigidBody>* bodyA, std::unique_ptr<RigidBody>* bodyB);
	static void polygonPolygon(RigidPolygon& a, RigidPolygon& b, std::unique_ptr<RigidBody>* bodyA, std::unique_ptr<RigidBody>* bodyB);
	static void circlePolygon(RigidCircle& a, RigidPolygon& b, std::unique_ptr<RigidBody>* bodyA, std::unique_ptr<RigidBody>* bodyB);
public:
	static void checkCollision(std::unique_ptr<RigidBody>& bodyA, std::unique_ptr<RigidBody>& bodyB);

	static const std::vector<CollisionManifold>& getManifolds();
	static bool areAnyCollisionsFound();
	static void clearManifolds();
};

