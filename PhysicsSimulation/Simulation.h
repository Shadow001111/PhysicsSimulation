#pragma once
#include <vector>

#include "RigidCircle.h"

class Simulation
{
	// Simulation parameters
	float fixedTimeStep = 1.0f / 300.0f;
	float worldBoundary = 1.0;
	float gravity = -9.81f;

	//
	float accumulatedUpdateTime = 0.0;

	std::vector<RigidCircle> circles;

	void singlePhysicsStep();
public:
	Simulation() = default;

	// Circles
	void addCircle(const glm::vec2& position, const glm::vec2& velocity, float radius);
	const std::vector<RigidCircle>& getCircles() const;

	// Simulation
	int update(float deltaTime);
	float calculateEnergy() const;
};

