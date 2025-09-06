#include "Simulation.h"
#include "math.h"
#include <iostream>

void Simulation::singlePhysicsStep()
{
	glm::vec2 acceleration(0.0f, gravity);
	for (auto& circle : circles)
	{
		// Update velocity and position
		circle.velocity += acceleration * fixedTimeStep;
		circle.position += circle.velocity * fixedTimeStep;

		// Boundary collision
		// TODO: Should change velocity based on acceleation and displacement.
		// Displacement proved to save energy better.
		if (circle.position.x - circle.radius < -worldBoundary)
		{
			float displacement = (circle.position.x - circle.radius) + worldBoundary;

			circle.position.x = -worldBoundary + circle.radius - displacement;
			circle.velocity.x = -circle.velocity.x;
		}
		else if (circle.position.x + circle.radius > worldBoundary)
		{
			float displacement = (circle.position.x + circle.radius) - worldBoundary;

			circle.position.x = worldBoundary - circle.radius - displacement;
			circle.velocity.x = -circle.velocity.x;
		}

		if (circle.position.y - circle.radius < -worldBoundary)
		{
			float displacement = (circle.position.y - circle.radius) + worldBoundary;

			circle.position.y = -worldBoundary + circle.radius - displacement;
			circle.velocity.y = -circle.velocity.y;
		}
		else if (circle.position.y + circle.radius > worldBoundary)
		{
			float displacement = (circle.position.y + circle.radius) - worldBoundary;

			circle.position.y = worldBoundary - circle.radius - displacement;
			circle.velocity.y = -circle.velocity.y;
		}
	}
}


void Simulation::addCircle(const glm::vec2& position, const glm::vec2& velocity, float radius)
{
	circles.emplace_back(position, velocity, radius);
}

const std::vector<RigidCircle>& Simulation::getCircles() const
{
	return circles;
}


int Simulation::update(float deltaTime)
{
	accumulatedUpdateTime += deltaTime;
	int updatesToPerform = floorf(accumulatedUpdateTime / fixedTimeStep);
	if (updatesToPerform <= 0)
	{
		return 0;
	}

	accumulatedUpdateTime -= updatesToPerform * fixedTimeStep;

	for (int i = 0; i < updatesToPerform; i++)
	{
		singlePhysicsStep();
	}

	return updatesToPerform;
}

float Simulation::calculateEnergy() const
{
	float totalEnergy = 0.0f;
	for (const auto& circle : circles)
	{
		float mass = 1.0f;
		float h = (circle.position.y - circle.radius) + worldBoundary; // Height above the bottom boundary

		float kineticEnergy = 0.5f * mass * glm::dot(circle.velocity, circle.velocity);
		float potentialEnergy = mass * -gravity * h;
		totalEnergy += kineticEnergy + potentialEnergy;
	}
	return totalEnergy;
}
