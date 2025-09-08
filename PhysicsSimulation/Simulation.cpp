#include "Simulation.h"
#include "math.h"
#include <iostream>
#include "Collisions.h"

void Simulation::singlePhysicsStep()
{
	updateOrientationAndVelocity();
	resolveCollisions();
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
	}
}

void Simulation::resolveCollisions()
{
	for (unsigned int iterations = 0; iterations < iterationsToSolveCollisions; iterations++)
	{
		bool anyCollisionHappened = resolveCollisionsSingleStep();
		if (!anyCollisionHappened)
		{
			break;
		}
	}
}

bool Simulation::resolveCollisionsSingleStep()
{
	bool anyCollisionHappened = false;

	size_t count = bodies.size();
	for (size_t index1 = 0; index1 < count - 1; index1++)
	{
		auto& body1 = bodies[index1];
		bool isBody1Static = body1->isStatic();
		for (size_t index2 = index1 + 1; index2 < count; index2++)
		{
			auto& body2 = bodies[index2];
			bool isBody2Static = body2->isStatic();

			if (isBody1Static && isBody2Static)
			{
				continue;
			}

			CollisionInfo collisionInfo = checkCollision(body1, body2);

			if (collisionInfo.depth <= 0.0f)
			{
				continue;
			}

			anyCollisionHappened = true;

			float elasticity = 1.0f + fminf(body1->elasticity, body2->elasticity);
			glm::vec2 relVel = body2->velocity - body1->velocity;
			float velAlongNormal = glm::dot(relVel, collisionInfo.normal);
			float j = elasticity * velAlongNormal / (body1->invMass + body2->invMass);

			glm::vec2 force = j * collisionInfo.normal;
			body1->velocity += force * body1->invMass;
			body2->velocity -= force * body2->invMass;

			glm::vec2 displacement = collisionInfo.normal * collisionInfo.depth;

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
	return anyCollisionHappened;
}

CollisionInfo Simulation::checkCollision(std::unique_ptr<RigidBody>& body1, std::unique_ptr<RigidBody>& body2) const
{
	CollisionInfo collisionInfo;

	if (body1->shapeType == ShapeType::Circle && body2->shapeType == ShapeType::Circle)
	{
		collisionInfo = Collisions::circleCircle(body1, body2);
	}
	else if (body1->shapeType == ShapeType::Polygon && body2->shapeType == ShapeType::Polygon)
	{
		collisionInfo = Collisions::polygonPolygon(body1, body2);
	}
	else if (body1->shapeType == ShapeType::Circle && body2->shapeType == ShapeType::Polygon)
	{
		collisionInfo = Collisions::circlePolygon(body1, body2);
	}
	else if (body1->shapeType == ShapeType::Polygon && body2->shapeType == ShapeType::Circle)
	{
		collisionInfo = Collisions::circlePolygon(body2, body1);
		collisionInfo.normal = -collisionInfo.normal;
	}

	return collisionInfo;
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