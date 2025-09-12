#pragma once
#include <vector>
#include <memory>

#include "Collisions.h"

class Simulation
{
	// Simulation parameters
	float fixedTimeStep = 1.0f / 300.0f;
	unsigned int iterationsToSolveCollisions = 1;
	unsigned int maxIterationsPerFrame = 32;

	float gravity = -9.81f;

	//
	float accumulatedUpdateTime = 0.0;

	std::vector<std::unique_ptr<RigidBody>> bodies;

	void singlePhysicsStep();
	void updateOrientationAndVelocity();
	void detectCollisions();
	void resolveCollisionsSingleStep();
public:
	Simulation() = default;

	// Bodies
	void addCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, const Material& material, float radius);
	void addBox(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, const Material& material, const glm::vec2& size);
	void addPolygon(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, const Material& material, const std::vector<glm::vec2>& vertices);

	const std::vector<std::unique_ptr<RigidBody>>& getBodies() const;

	// Simulation
	int update(float deltaTime);
};

