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
		if (body->isStatic())
		{
			continue;
		}

		// Update orientation and velocity
		body->velocity += acceleration * fixedTimeStep;
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
		const AABB& aabb1 = body1->getAABB();
		const bool isBody1Static = body1->isStatic();
		for (size_t index2 = index1 + 1; index2 < count; index2++)
		{
			auto& body2 = bodies[index2];
			const bool isBody2Static = body2->isStatic();

			if (isBody1Static && isBody2Static)
			{
				continue;
			}

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

		//
		float elasticityPlusOne = 1.0f + fminf(body1->material.elasticity, body2->material.elasticity);
		float staticFriction = (body1->material.staticFriction + body2->material.staticFriction) * 0.5f;
		float dynamicFriction = (body1->material.dynamicFriction + body2->material.dynamicFriction) * 0.5f;

		glm::vec2 impulses[2];
		glm::vec2 r1s[2];
		glm::vec2 r2s[2];
		float js[2] = { 0.0f, 0.0f };

		// TODO: Try using perpendicular 'rs' and 'dot' function, instead of using 'cross'

		// Calculate impulses
		unsigned int countOfImpulses = 0;
		impulses[0] = { 0.0f, 0.0f };
		impulses[1] = { 0.0f, 0.0f };
		for (unsigned int i = 0; i < manifold.countOfContacts; i++)
		{
			const glm::vec2& contact = manifold.contacts[i];

			glm::vec2 r1 = contact - body1->position;
			glm::vec2 r2 = contact - body2->position;
			r1s[i] = r1;
			r2s[i] = r2;

			glm::vec2 r1Perp = glm::vec2(-r1.y, r1.x);
			glm::vec2 r2Perp = glm::vec2(-r2.y, r2.x);

			glm::vec2 angularLinearVelocity1 = r1Perp * body1->angularVelocity;
			glm::vec2 angularLinearVelocity2 = r2Perp * body2->angularVelocity;

			glm::vec2 relativeVelocity = (body2->velocity + angularLinearVelocity2) - (body1->velocity + angularLinearVelocity1);

			float velAlongNormal = glm::dot(relativeVelocity, manifold.normal);
			if (velAlongNormal > 0.0f)
			{
				continue;
			}

			float r1PerpDotN = glm::dot(r1Perp, manifold.normal);
			float r2PerpDotN = glm::dot(r2Perp, manifold.normal);

			float mass_ = body1->invMass + body2->invMass;
			float inertia1_ = r1PerpDotN * r1PerpDotN * body1->invInertia;
			float inertia2_ = r2PerpDotN * r2PerpDotN * body2->invInertia;

			float denom = mass_ + inertia1_ + inertia2_;
			float j = elasticityPlusOne * velAlongNormal / denom;

			impulses[countOfImpulses]= j * manifold.normal;
			js[i] = j;
			impulses[countOfImpulses++];
		}

		// Apply impulses
		for (unsigned int i = 0; i < countOfImpulses; i++)
		{
			const auto& impulse = impulses[i] / (float)countOfImpulses;
			const auto& r1 = r1s[i];
			const auto& r2 = r2s[i];

			body1->velocity += impulse * body1->invMass;
			float cross1 = r1.x * impulse.y - r1.y * impulse.x;
			body1->angularVelocity += cross1 * body1->invInertia;

			body2->velocity -= impulse * body2->invMass;
			float cross2 = r2.x * impulse.y - r2.y * impulse.x;
			body2->angularVelocity -= cross2 * body2->invInertia;
		}

		// Calculate impulses
		countOfImpulses = 0;
		impulses[0] = { 0.0f, 0.0f };
		impulses[1] = { 0.0f, 0.0f };
		for (unsigned int i = 0; i < manifold.countOfContacts; i++)
		{
			const glm::vec2& contact = manifold.contacts[i];

			glm::vec2 r1 = r1s[i];
			glm::vec2 r2 = r2s[i];

			glm::vec2 r1Perp = glm::vec2(-r1.y, r1.x);
			glm::vec2 r2Perp = glm::vec2(-r2.y, r2.x);

			glm::vec2 angularLinearVelocity1 = r1Perp * body1->angularVelocity;
			glm::vec2 angularLinearVelocity2 = r2Perp * body2->angularVelocity;

			glm::vec2 relativeVelocity = (body2->velocity + angularLinearVelocity2) - (body1->velocity + angularLinearVelocity1);

			glm::vec2 tangent = relativeVelocity - glm::dot(relativeVelocity, manifold.normal) * manifold.normal;
			if (glm::dot(tangent, tangent) < 1e-16f)
			{
				continue;
			}

			tangent = glm::normalize(tangent);

			float r1PerpDotT = glm::dot(r1Perp, tangent);
			float r2PerpDotT = glm::dot(r2Perp, tangent);

			float mass_ = body1->invMass + body2->invMass;
			float inertia1_ = r1PerpDotT * r1PerpDotT * body1->invInertia;
			float inertia2_ = r2PerpDotT * r2PerpDotT * body2->invInertia;

			float denom = (mass_ + inertia1_ + inertia2_) * (float)manifold.countOfContacts;
			float jt = -glm::dot(relativeVelocity, tangent) / denom;

			float j = js[i];
			if (j == 0.0f)
			{
				continue;
			}

			glm::vec2 frictionImpulse;
			if (fabsf(jt) <= j * staticFriction)
			{
				frictionImpulse = jt * tangent;
			}
			else
			{
				frictionImpulse = -j * dynamicFriction * tangent;
			}

			impulses[countOfImpulses] = frictionImpulse;
			countOfImpulses++;
		}

		// Apply impulses
		for (unsigned int i = 0; i < countOfImpulses; i++)
		{
			const auto& impulse = impulses[i] / (float)countOfImpulses;
			const auto& r1 = r1s[i];
			const auto& r2 = r2s[i];

			body1->velocity += impulse * body1->invMass;
			float cross1 = r1.x * impulse.y - r1.y * impulse.x;
			body1->angularVelocity += cross1 * body1->invInertia;

			body2->velocity -= impulse * body2->invMass;
			float cross2 = r2.x * impulse.y - r2.y * impulse.x;
			body2->angularVelocity -= cross2 * body2->invInertia;
		}
	
		// Separate bodies
		glm::vec2 displacement = manifold.normal * manifold.depth;

		if (body1->isStatic())
		{
			body2->move(displacement);
		}
		else if (body2->isStatic())
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

void Simulation::addCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, const Material& material, float radius)
{
	bodies.push_back(std::make_unique<RigidCircle>(pos, vel, rot, angVel, mass, inertia, material, radius));
}

void Simulation::addBox(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, const Material& material, const glm::vec2& size)
{
	float w = size.x * 0.5f;
	float h = size.y * 0.5f;

	std::vector<glm::vec2> vertices =
	{
		{-w, h}, {w, h}, {w, -h}, {-w, -h}
	};

	bodies.push_back(std::make_unique<RigidPolygon>(pos, vel, rot, angVel, mass, inertia, material, vertices));
}

const std::vector<std::unique_ptr<RigidBody>>& Simulation::getBodies() const
{
	return bodies;
}


int Simulation::update(float deltaTime)
{
	accumulatedUpdateTime += deltaTime;
	unsigned int updatesToPerform = floorf(accumulatedUpdateTime / fixedTimeStep);
	updatesToPerform = std::min(updatesToPerform, maxIterationsPerFrame);
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