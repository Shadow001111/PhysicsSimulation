#pragma once
#include <vector>
#include <memory>

#include "Collisions.h"

class Simulation
{
	// Simulation parameters
	float fixedTimeStep = 1.0f / 300.0f;
	unsigned int iterationsToSolveCollisions = 1;

	float gravity = -9.81f;

	//
	float accumulatedUpdateTime = 0.0;

	std::vector<std::unique_ptr<RigidBody>> bodies;

	void singlePhysicsStep();
	void updateOrientationAndVelocity();
	void resolveCollisions();
	bool resolveCollisionsSingleStep();
	CollisionInfo checkCollision(std::unique_ptr<RigidBody>& body1, std::unique_ptr<RigidBody>& body2) const;
public:
	Simulation() = default;

	// Bodies
	void addCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float elasticity, float radius);
	void addBox(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float elasticity, const glm::vec2& size);

	const std::vector<std::unique_ptr<RigidBody>>& getBodies() const;

	// Simulation
	int update(float deltaTime);
};

