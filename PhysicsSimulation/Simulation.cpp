#include "Simulation.h"
#include "math.h"
#include <iostream>

void Simulation::singlePhysicsStep()
{
	glm::vec2 acceleration(0.0f, gravity);
	for (auto& circle : bodies)
	{
		// Update velocity and position
		circle->velocity += acceleration * fixedTimeStep;
		circle->position += circle->velocity * fixedTimeStep;

		// Boundary collision
		// TODO: Should change velocity based on acceleation and displacement.
		// Displacement proved to save energy better.
		if (circle->position.x < -worldBoundary)
		{
			float displacement = circle->position.x + worldBoundary;

			circle->position.x = -worldBoundary - displacement;
			circle->velocity.x = -circle->velocity.x;
		}
		else if (circle->position.x > worldBoundary)
		{
			float displacement = circle->position.x - worldBoundary;

			circle->position.x = worldBoundary - displacement;
			circle->velocity.x = -circle->velocity.x;
		}

		if (circle->position.y < -worldBoundary)
		{
			float displacement = circle->position.y + worldBoundary;

			circle->position.y = -worldBoundary - displacement;
			circle->velocity.y = -circle->velocity.y;
		}
		else if (circle->position.y > worldBoundary)
		{
			float displacement = circle->position.y - worldBoundary;

			circle->position.y = worldBoundary - displacement;
			circle->velocity.y = -circle->velocity.y;
		}
	}
}


void Simulation::addCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float radius, float mass, float elasticity)
{
	bodies.push_back(std::make_unique<RigidCircle>(pos, vel, rot, angVel, radius, mass, elasticity));
}

const std::vector<std::unique_ptr<RigidBody>>& Simulation::getBodies() const
{
	return bodies;
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
	// Doesn't care about shape size
	float totalEnergy = 0.0f;
	for (const auto& body : bodies)
	{
		float mass = 1.0f;
		float h = body->position.y + worldBoundary; // Height above the bottom boundary

		float kineticEnergy = 0.5f * mass * glm::dot(body->velocity, body->velocity);
		float potentialEnergy = mass * -gravity * h;
		totalEnergy += kineticEnergy + potentialEnergy;
	}
	return totalEnergy;
}
