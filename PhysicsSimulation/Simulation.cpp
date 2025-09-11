#include "Simulation.h"
#include "math.h"
#include <iostream>
#include "Collisions.h"

void Simulation::singlePhysicsStep()
{
	updateOrientationAndVelocity();

	// Collisions
	for (unsigned int i = 0; i < iterationsToSolveCollisions; i++)
	{
		detectCollisions();
		if (!Collisions::areAnyCollisionsFound())
		{
			break;
		}
		resolveCollisionsSingleStep();
	}
}

void Simulation::updateOrientationAndVelocity()
{
	glm::vec2 acceleration(0.0f, gravity);
	for (auto& body : bodies)
	{
		// Update orientation and velocity
		if (!body->isStatic())
		{
			body->velocity += acceleration * fixedTimeStep;
		}

		body->position += body->velocity * fixedTimeStep;
		body->rotation += body->angularVelocity * fixedTimeStep;

		// TODO: maybe use 'move' and 'rotate' method
		body->transformUpdateRequired = true;
		body->aabbUpdateRequired = true;
	}
}

void Simulation::detectCollisions()
{
	Collisions::clearManifolds();

	size_t count = bodies.size();
	for (size_t index1 = 0; index1 < count - 1; index1++)
	{
		auto& body1 = bodies[index1];
		const bool isBody1Static = body1->isStatic();
		for (size_t index2 = index1 + 1; index2 < count; index2++)
		{
			auto& body2 = bodies[index2];
			const bool isBody2Static = body2->isStatic();

			if (isBody1Static && isBody2Static)
			{
				continue;
			}

			const AABB& aabb1 = body1->getAABB();
			const AABB& aabb2 = body2->getAABB();
			if (!aabb1.isIntersecting(aabb2))
			{
				continue;
			}

			Collisions::checkCollision(body1, body2);
		}
	}
}

void Simulation::resolveCollisionsSingleStep()
{
	const auto& manifolds = Collisions::getManifolds();
	for (auto& manifold : manifolds)
	{
		auto& body1 = *manifold.bodyA;
		auto& body2 = *manifold.bodyB;

		bool isBody1Static = body1->isStatic();
		bool isBody2Static = body2->isStatic();

		float elasticity = 1.0f + fminf(body1->elasticity, body2->elasticity);
		glm::vec2 relVel = body2->velocity - body1->velocity;
		float velAlongNormal = glm::dot(relVel, manifold.normal);
		float j = elasticity * velAlongNormal / (body1->invMass + body2->invMass);

		glm::vec2 force = j * manifold.normal;
		body1->velocity += force * body1->invMass;
		body2->velocity -= force * body2->invMass;

		glm::vec2 displacement = manifold.normal * manifold.depth;

		if (isBody1Static)
		{
			body2->move(displacement);
		}
		else if (isBody2Static)
		{
			body1->move(-displacement);
		}
		else
		{
			float displacementRatio = body1->mass / (body1->mass + body2->mass);
			body1->move(displacement * -(1.0f - displacementRatio));
			body2->move(displacement * displacementRatio);
		}
	}
}

void Simulation::addCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float elasticity, float radius)
{
	bodies.push_back(std::make_unique<RigidCircle>(pos, vel, rot, angVel, mass, elasticity, radius));
}

void Simulation::addBox(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float elasticity, const glm::vec2& size)
{
	float w = size.x * 0.5f;
	float h = size.y * 0.5f;

	std::vector<glm::vec2> vertices =
	{
		{-w, h}, {w, h}, {w, -h}, {-w, -h}
	};

	bodies.push_back(std::make_unique<RigidPolygon>(pos, vel, rot, angVel, mass, elasticity, vertices));
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