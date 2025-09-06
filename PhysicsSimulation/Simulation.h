#pragma once
#include <vector>
#include <memory>

#include "RigidCircle.h"

class Simulation
{
	// Simulation parameters
	float fixedTimeStep = 1.0f / 300.0f;
	float worldBoundary = 1.0;
	float gravity = -9.81f;

	//
	float accumulatedUpdateTime = 0.0;

	std::vector<std::unique_ptr<RigidBody>> bodies;

	void singlePhysicsStep();
public:
	Simulation() = default;

	// Bodies
	void addCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float radius, float mass, float elasticity);

	const std::vector<std::unique_ptr<RigidBody>>& getBodies() const;

	// Simulation
	int update(float deltaTime);
	float calculateEnergy() const;
};

