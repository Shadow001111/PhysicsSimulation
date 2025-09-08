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
	size_t count = bodies.size();

	for (unsigned int iterations = 0; iterations < iterationsToSolveCollisions; iterations++)
	{
		bool anyCollisionHappened = false;
		for (size_t i = 0; i < count - 1; i++)
		{
			auto& body1 = bodies[i];
			bool isBody1Static = body1->isStatic();
			for (size_t j = i + 1; j < count; j++)
			{
				auto& body2 = bodies[j];
				bool isBody2Static = body2->isStatic();

				if (isBody1Static && isBody2Static)
				{
					continue;
				}

				CollisionInfo collisionInfo;

				if (body1->shapeType == ShapeType::Circle && body2->shapeType == ShapeType::Circle)
				{
					collisionInfo  = Collisions::circleCircle(body1, body2);
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

				if (collisionInfo.depth > 0.0f)
				{
					anyCollisionHappened = true;

					float elasticity = 1.0f + fminf(body1->elasticity, body2->elasticity);
					glm::vec2 relVel = body2->velocity - body1->velocity;
					float velAlongNormal = glm::dot(relVel, collisionInfo.normal);
					float j = elasticity * velAlongNormal / (body1->invMass + body2->invMass);
					body1->velocity += (j * body1->invMass) * collisionInfo.normal;
					body2->velocity -= (j * body2->invMass) * collisionInfo.normal;

					if (isBody1Static)
					{
						body2->move(collisionInfo.normal * collisionInfo.depth);
					}
					else if (isBody2Static)
					{
						body1->move(collisionInfo.normal * -collisionInfo.depth);
					}
					else
					{
						float body2DisplacementRatio = body1->mass / (body1->mass + body1->mass);
						float body1DisplacementRatio = 1.0f - body2DisplacementRatio;
						body1->move(collisionInfo.normal * (collisionInfo.depth * -body1DisplacementRatio));
						body2->move(collisionInfo.normal * (collisionInfo.depth * body2DisplacementRatio));
					}
				}
			}
		}
		if (!anyCollisionHappened)
		{
			break;
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