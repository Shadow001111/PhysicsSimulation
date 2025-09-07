#include "Simulation.h"
#include "math.h"
#include <iostream>
#include "Collisions.h"

void Simulation::singlePhysicsStep()
{
	updatePositionAndVelocity();
	resolveCollisions();
}

void Simulation::updatePositionAndVelocity()
{
	glm::vec2 acceleration(0.0f, gravity);
	for (auto& body : bodies)
	{
		// Update velocity and position
		body->velocity += acceleration * fixedTimeStep;
		body->position += body->velocity * fixedTimeStep;

		// Boundary collision
		// TODO: Should change velocity based on acceleation and displacement.
		// Displacement proved to save energy better.
		if (body->position.x < -worldBoundary)
		{
			float displacement = body->position.x + worldBoundary;

			body->position.x = -worldBoundary - displacement;
			body->velocity.x = -body->velocity.x * body->elasticity;
		}
		else if (body->position.x > worldBoundary)
		{
			float displacement = body->position.x - worldBoundary;

			body->position.x = worldBoundary - displacement;
			body->velocity.x = -body->velocity.x * body->elasticity;
		}

		if (body->position.y < -worldBoundary)
		{
			float displacement = body->position.y + worldBoundary;

			body->position.y = -worldBoundary - displacement;
			body->velocity.y = -body->velocity.y * body->elasticity;
		}
		else if (body->position.y > worldBoundary)
		{
			float displacement = body->position.y - worldBoundary;

			body->position.y = worldBoundary - displacement;
			body->velocity.y = -body->velocity.y * body->elasticity;
		}
	}
}

void Simulation::resolveCollisions()
{
	size_t count = bodies.size();

	for (unsigned int iterations = 0; iterations < iterationsToSolveCollisions; iterations++)
	{
		bool anyCollision = false;
		for (size_t i = 0; i < count - 1; i++)
		{
			auto& body1 = bodies[i];
			auto& circle1 = *(dynamic_cast<RigidCircle*>(body1.get()));
			for (size_t j = i + 1; j < count; j++)
			{
				auto& body2 = bodies[j];
				auto& circle2 = *(dynamic_cast<RigidCircle*>(body2.get()));

				anyCollision |= Collisions::circleCircle(circle1, circle2);
			}
		}
		if (!anyCollision)
		{
			break;
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
