#include "Simulation.h"
#include "Collisions.h"
#include "Profiler.h"

#include "math.h"
#include <iostream>
#include "SpringConstraint.h"

void Simulation::singlePhysicsStep()
{
	updateOrientationAndVelocity();

	// Collisions
	{
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
		body->moveAndRotate(body->velocity * fixedTimeStep, body->angularVelocity * fixedTimeStep);
	}
}

void Simulation::detectCollisions()
{
	Collisions::clearManifolds();

	switch (collisionMethod)
	{
	case CollisionDetectionMethod::BruteForce:
		detectCollisionsBruteForce();
		break;
	case CollisionDetectionMethod::Quadtree:
		detectCollisionsWithQuadtree();
		break;
	case CollisionDetectionMethod::SpatialHashGrid:
		detectCollisionsWithHashGrid();
		break;
	}
}

void Simulation::detectCollisionsBruteForce()
{
	size_t count = bodies.size();
	if (count < 2)
	{
		return;
	}

	PROFILE_SCOPE("Brute Force Pair Testing");

	for (size_t i = 0; i < count - 1; i++)
	{
		auto& bodyA = bodies[i];
		const bool isBodyAStatic = bodyA->isStatic();
		const AABB& bodyA_AABB = bodyA->getAABB_noUpdate();

		for (size_t j = i + 1; j < count; j++)
		{
			auto& bodyB = bodies[j];
			const bool isBodyBStatic = bodyB->isStatic();

			if (isBodyAStatic && isBodyBStatic)
			{
				continue;
			}

			const AABB& bodyB_AABB = bodyB->getAABB_noUpdate();
			if (!bodyA_AABB.isIntersecting(bodyB_AABB))
			{
				continue;
			}

			Collisions::checkCollision(bodyA, bodyB);
		}
	}
}

void Simulation::detectCollisionsWithQuadtree()
{
	// Rebuild quadtree with current body positions
	quadtree->rebuild(bodies);

	// Get potential collision pairs from quadtree
	static std::vector<RigidBodyPair> pairs;
	pairs.clear();
	quadtree->getPotentialCollisions(pairs);

	// Check actual collisions for potential pairs
	{
		PROFILE_SCOPE("Quadtree Narrow Phase");

		for (const auto& pair : pairs)
		{
			RigidBody* body1 = pair.first;
			RigidBody* body2 = pair.second;
			Collisions::checkCollision(body1, body2);
		}
	}
}

void Simulation::detectCollisionsWithHashGrid()
{
	// Rebuild hash grid with current body positions
	spatialHashGrid->rebuild(bodies);

	// Get potential collision pairs from hash grid
	static std::vector<RigidBodyPair> pairs;
	pairs.clear();
	spatialHashGrid->getPotentialCollisions(pairs);

	// Check actual collisions for potential pairs
	{
		PROFILE_SCOPE("Hash Grid Narrow Phase");

		for (const auto& pair : pairs)
		{
			RigidBody* body1 = pair.first;
			RigidBody* body2 = pair.second;
			Collisions::checkCollision(body1, body2);
		}
	}
}

void Simulation::resolveCollisionsSingleStep()
{
	const auto& manifolds = Collisions::getManifolds();
	for (auto& manifold : manifolds)
	{
		RigidBody* body1 = manifold.bodyA;
		RigidBody* body2 = manifold.bodyB;

		// Cache frequently used values
		const float elasticityPlusOne = 1.0f + fminf(body1->material->elasticity, body2->material->elasticity);
		const float staticFriction = (body1->material->staticFriction + body2->material->staticFriction) * 0.5f;
		const float dynamicFriction = (body1->material->dynamicFriction + body2->material->dynamicFriction) * 0.5f;
		const float invMassSum = body1->invMass + body2->invMass;

		//
		glm::vec2 impulses[2] = { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
		glm::vec2 perpR1Array[2] = { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
		glm::vec2 perpR2Array[2] = { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
		float jnArray[2] = { 0.0f, 0.0f };
		unsigned int validContacts = 0;

		// TODO: Maybe remove repetitive code of applying impulses

		// Calculate collision impulses
		for (unsigned int i = 0; i < manifold.countOfContacts; i++)
		{
			const glm::vec2& contact = manifold.contacts[i];

			glm::vec2 r1 = contact - body1->position;
			glm::vec2 r2 = contact - body2->position;

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

			float inertia1_ = r1PerpDotN * r1PerpDotN * body1->invInertia;
			float inertia2_ = r2PerpDotN * r2PerpDotN * body2->invInertia;

			float denom = invMassSum + inertia1_ + inertia2_;
			float jn = -elasticityPlusOne * velAlongNormal / denom;

			impulses[validContacts] = jn * manifold.normal;
			perpR1Array[validContacts] = r1Perp;
			perpR2Array[validContacts] = r2Perp;
			jnArray[validContacts] = jn;
			validContacts++;
		}

		// Apply collision impulses
		for (unsigned int i = 0; i < validContacts; i++)
		{
			const glm::vec2 impulse = impulses[i] / (float)validContacts;
			const glm::vec2& r1Perp = perpR1Array[i];
			const glm::vec2& r2Perp = perpR2Array[i];

			body1->velocity -= impulse * body1->invMass;
			body1->angularVelocity -= glm::dot(r1Perp, impulse) * body1->invInertia;

			body2->velocity += impulse * body2->invMass;
			body2->angularVelocity += glm::dot(r2Perp, impulse) * body2->invInertia;
		}

		// Calculate friction impulses
		for (unsigned int i = 0; i < validContacts; i++)
		{
			glm::vec2 r1Perp = perpR1Array[i];
			glm::vec2 r2Perp = perpR2Array[i];

			glm::vec2 angularLinearVelocity1 = r1Perp * body1->angularVelocity;
			glm::vec2 angularLinearVelocity2 = r2Perp * body2->angularVelocity;

			glm::vec2 relativeVelocity = (body2->velocity + angularLinearVelocity2) - (body1->velocity + angularLinearVelocity1);

			glm::vec2 tangent = relativeVelocity - glm::dot(relativeVelocity, manifold.normal) * manifold.normal;
			if (glm::dot(tangent, tangent) < 1e-16f)
			{
				impulses[i] = { 0.0f, 0.0f };
				continue;
			}

			tangent = glm::normalize(tangent);

			float r1PerpDotT = glm::dot(r1Perp, tangent);
			float r2PerpDotT = glm::dot(r2Perp, tangent);

			float inertia1_ = r1PerpDotT * r1PerpDotT * body1->invInertia;
			float inertia2_ = r2PerpDotT * r2PerpDotT * body2->invInertia;

			float denom = invMassSum + inertia1_ + inertia2_;
			float jt = -glm::dot(relativeVelocity, tangent) / denom;

			float jn = jnArray[i];
			if (fabsf(jt) <= jn * staticFriction)
			{
				impulses[i] = jt * tangent; // static friction
			}
			else
			{
				impulses[i] = -jn * dynamicFriction * tangent; // dynamic friction
			}
		}

		// TODO: Maybe apply impulses directly in above loop
		// Apply friction impulses
		for (unsigned int i = 0; i < validContacts; i++)
		{
			const auto& impulse = impulses[i] / (float)validContacts;
			const auto& r1Perp = perpR1Array[i];
			const auto& r2Perp = perpR2Array[i];

			body1->velocity -= impulse * body1->invMass;
			body1->angularVelocity -= glm::dot(r1Perp, impulse) * body1->invInertia;

			body2->velocity += impulse * body2->invMass;
			body2->angularVelocity += glm::dot(r2Perp, impulse) * body2->invInertia;
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

Simulation::Simulation()
{
	worldBounds = { glm::vec2(-WORLD_BOUNDS), glm::vec2(WORLD_BOUNDS) };
	quadtree = std::make_unique<Quadtree>(worldBounds);
	spatialHashGrid = std::make_unique<SpatialHashGrid>(worldBounds, 0.1f * 1.41f);
}

std::unique_ptr<RigidBody>& Simulation::addCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, float radius)
{
	bodies.push_back(std::make_unique<RigidCircle>(pos, vel, rot, angVel, mass, inertia, material, radius));
	return bodies.back();
}

std::unique_ptr<RigidBody>& Simulation::addBox(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, const glm::vec2& size)
{
	float w = size.x * 0.5f;
	float h = size.y * 0.5f;

	std::vector<glm::vec2> vertices =
	{
		{-w, h}, {w, h}, {w, -h}, {-w, -h}
	};

	bodies.push_back(std::make_unique<RigidPolygon>(pos, vel, rot, angVel, mass, inertia, material, vertices));
	return bodies.back();
}

std::unique_ptr<RigidBody>& Simulation::addPolygon(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, const std::vector<glm::vec2>& vertices)
{
	bodies.push_back(std::make_unique<RigidPolygon>(pos, vel, rot, angVel, mass, inertia, material, vertices));
	return bodies.back();
}

const std::vector<std::unique_ptr<RigidBody>>& Simulation::getBodies() const
{
	return bodies;
}

int Simulation::update(float deltaTime)
{
	Profiler::beginFrame();

	accumulatedUpdateTime += deltaTime;
	unsigned int updatesToPerform = floorf(accumulatedUpdateTime / fixedTimeStep);
	updatesToPerform = std::min(updatesToPerform, maxIterationsPerFrame);
	if (updatesToPerform <= 0)
	{
		Profiler::endFrame();
		return 0;
	}

	accumulatedUpdateTime -= updatesToPerform * fixedTimeStep;

	for (unsigned int i = 0; i < updatesToPerform; i++)
	{
		singlePhysicsStep();
	}

	Profiler::endFrame();
	return updatesToPerform;
}

void Simulation::setCollisionDetectionMethod(CollisionDetectionMethod method)
{
	collisionMethod = method;
}

CollisionDetectionMethod Simulation::getCollisionDetectionMethod() const
{
	return collisionMethod;
}

void Simulation::getQuadtreeBounds(std::vector<AABB>& bounds) const
{
	bounds.clear();
	if (quadtree)
	{
		quadtree->getAllBounds(bounds);
	}
}

void Simulation::getHashGridBounds(std::vector<AABB>& bounds, bool onlyActive) const
{
	bounds.clear();
	if (spatialHashGrid)
	{
		if (onlyActive)
		{
			spatialHashGrid->getActiveCellBounds(bounds);
		}
		else
		{
			spatialHashGrid->getAllCellBounds(bounds);
		}
	}
}

void Simulation::setSpatialHashGridCellSize(float cellSize)
{
	spatialHashGrid = std::make_unique<SpatialHashGrid>(worldBounds, cellSize);
}

void Simulation::printPerfomanceReport() const
{
	Profiler::printProfileReport();
	Profiler::resetAllProfiles();
}
