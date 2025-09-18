#include "Collisions.h"
#include "math.h"
#include <iostream>

const Collisions::CheckCollisionFunction Collisions::checkCollisionFunctionsMatrix[2][2] =
{
	// Circle                   Polygon
	{ Collisions::circleCircle, Collisions::circlePolygon  }, // Circle
	{ nullptr,                  Collisions::polygonPolygon }  // Polygon
};

std::vector<CollisionManifold> Collisions::manifolds;

glm::vec2 Collisions::projectVertices(const std::vector<glm::vec2>& vertices, glm::vec2 axis)
{
	float min = FLT_MAX;
	float max = -FLT_MAX;

	for (const auto& vert : vertices)
	{
		float proj = glm::dot(vert, axis);

		if (proj < min)
		{
			min = proj;
		}
		if (proj > max)
		{
			max = proj;
		}
	}
	return { min, max };
}

glm::vec2 Collisions::projectCircle(const glm::vec2& position, float radius, glm::vec2 axis)
{
	float proj = glm::dot(position, axis);
	return { proj - radius, proj + radius };
}

glm::vec2 Collisions::findClosestVertexOnPolygon(const glm::vec2& point, const std::vector<glm::vec2>& vertices)
{
	glm::vec2 closestPoint = {};
	float minDistSquared = FLT_MAX;

	for (const auto& vert : vertices)
	{
		glm::vec2 dpos = point - vert;
		float distSquared = glm::dot(dpos, dpos);
		if (distSquared < minDistSquared)
		{
			minDistSquared = distSquared;
			closestPoint = vert;
		}
	}
	return closestPoint;
}

glm::vec2 Collisions::findClosestPointOnSegment(const glm::vec2& start, const glm::vec2& end, const glm::vec2& point, float& outDistanceSquared)
{
	glm::vec2 startEnd = end - start;
	glm::vec2 startPoint = point - start;

	float proj = glm::dot(startEnd, startPoint);
	float lengthSquared = glm::dot(startEnd, startEnd);
	float d = proj / lengthSquared;

	glm::vec2 contact;
	if (d <= 0.0f)
	{
		contact = start;
	}
	else if (d >= 1.0f)
	{
		contact = end;
	}
	else
	{
		contact = start + startEnd * d;
	}

	glm::vec2 dpos = contact - point;
	outDistanceSquared = glm::dot(dpos, dpos);

	return contact;
}

bool Collisions::circleCircle(CollisionManifold& result, const RigidBody* bodyA, const RigidBody* bodyB)
{
	const RigidCircle* circleA = dynamic_cast<const RigidCircle*>(bodyA);
	const RigidCircle* circleB = dynamic_cast<const RigidCircle*>(bodyB);

	glm::vec2 deltaPos = circleB->position - circleA->position;
	float distanceSquared = glm::dot(deltaPos, deltaPos);

	float radiusSum = circleA->radius + circleB->radius;
	if (distanceSquared >= radiusSum * radiusSum)
	{
		return false;
	}

	float distance = sqrtf(distanceSquared);
	glm::vec2 normal;
	float depth = radiusSum - distance;

	if (distanceSquared == 0.0f)
	{
		normal = { 0.0f, 1.0f };
	}
	else
	{
		normal = deltaPos / distance;
	}

	result.normal = normal;
	result.depth = depth;
	result.contacts[0] = circleA->position + normal * circleA->radius;
	result.countOfContacts = 1;
	return true;
}

bool Collisions::polygonPolygon(CollisionManifold& result, const RigidBody* bodyA, const RigidBody* bodyB)
{
	const RigidPolygon* polygonA = dynamic_cast<const RigidPolygon*>(bodyA);
	const RigidPolygon* polygonB = dynamic_cast<const RigidPolygon*>(bodyB);

	glm::vec2 normal = {};
	float depth = FLT_MAX;

	const auto& verticesA = polygonA->getTransformedVertices();
	const auto& verticesB = polygonB->getTransformedVertices();

	const size_t verticesCountA = verticesA.size();
	const size_t verticesCountB = verticesB.size();

	// Check if bodies are colliding
	{
		bool collisionSide = false;
		for (size_t i = 0; i < verticesCountA; i++)
		{
			glm::vec2 va = verticesA[i];
			glm::vec2 vb = verticesA[(i + 1) % verticesCountA];

			glm::vec2 edge = vb - va;
			glm::vec2 axis = glm::normalize(glm::vec2(-edge.y, edge.x));

			glm::vec2 rangeA = projectVertices(verticesA, axis);
			glm::vec2 rangeB = projectVertices(verticesB, axis);

			if (rangeA.x >= rangeB.y || rangeB.x >= rangeA.y)
			{
				return false;
			}

			float bmax_amin = rangeB.y - rangeA.x;
			float amax_bmin = rangeA.y - rangeB.x;
			float axisDepth = fminf(bmax_amin, amax_bmin);
			if (axisDepth < depth)
			{
				depth = axisDepth;
				normal = axis;
				collisionSide = bmax_amin < amax_bmin;
			}
		}

		for (size_t i = 0; i < verticesCountB; i++)
		{
			glm::vec2 va = verticesB[i];
			glm::vec2 vb = verticesB[(i + 1) % verticesCountB];

			glm::vec2 edge = vb - va;
			glm::vec2 axis = glm::normalize(glm::vec2(-edge.y, edge.x));

			glm::vec2 rangeA = projectVertices(verticesA, axis);
			glm::vec2 rangeB = projectVertices(verticesB, axis);

			if (rangeA.x >= rangeB.y || rangeB.x >= rangeA.y)
			{
				return false;
			}

			float bmax_amin = rangeB.y - rangeA.x;
			float amax_bmin = rangeA.y - rangeB.x;
			float axisDepth = fminf(bmax_amin, amax_bmin);
			if (axisDepth < depth)
			{
				depth = axisDepth;
				normal = axis;
				collisionSide = bmax_amin < amax_bmin;
			}
		}

		if (collisionSide)
		{
			normal = -normal;
		}
	}

	// Find contact points
	glm::vec2 contact1, contact2;
	unsigned int countOfContacts = 1;
	{
		float minDistanceSquared = FLT_MAX;
		for (size_t i = 0; i < verticesCountA; i++)
		{
			glm::vec2 p = verticesA[i];

			for (size_t j = 0; j < verticesCountB; j++)
			{
				glm::vec2 va = verticesB[j];
				glm::vec2 vb = verticesB[(j + 1) % verticesCountB];
			
				float distanceSquared;
				glm::vec2 contact = findClosestPointOnSegment(va, vb, p, distanceSquared);

				if (fabsf(distanceSquared - minDistanceSquared) < 1e-6f)
				{
					glm::vec2 diff = contact - contact2;
					if (glm::dot(diff, diff) > 1e-16f)
					{
						contact2 = contact;
						countOfContacts = 2;
					}
				}
				else if (distanceSquared < minDistanceSquared)
				{
					minDistanceSquared = distanceSquared;
					contact1 = contact;
					countOfContacts = 1;
				}
			}
		}
		for (size_t i = 0; i < verticesCountB; i++)
		{
			glm::vec2 p = verticesB[i];

			for (size_t j = 0; j < verticesCountA; j++)
			{
				glm::vec2 va = verticesA[j];
				glm::vec2 vb = verticesA[(j + 1) % verticesCountA];

				float distanceSquared;
				glm::vec2 contact = findClosestPointOnSegment(va, vb, p, distanceSquared);

				if (fabsf(distanceSquared - minDistanceSquared) < 1e-6f)
				{
					glm::vec2 diff = contact - contact2;
					if (glm::dot(diff, diff) > 1e-16f)
					{
						contact2 = contact;
						countOfContacts = 2;
					}
				}
				else if (distanceSquared < minDistanceSquared)
				{
					minDistanceSquared = distanceSquared;
					contact1 = contact;
					countOfContacts = 1;
				}
			}
		}
	}

	result.normal = normal;
	result.depth = depth;
	result.contacts[0] = contact1;
	result.contacts[1] = contact2;
	result.countOfContacts = countOfContacts;
}

bool Collisions::circlePolygon(CollisionManifold& result, const RigidBody* bodyA, const RigidBody* bodyB)
{
	const RigidCircle* circleA = dynamic_cast<const RigidCircle*>(bodyA);
	const RigidPolygon* polygonB = dynamic_cast<const RigidPolygon*>(bodyB);

	glm::vec2 normal = {};
	float depth = FLT_MAX;

	const auto& vertices = polygonB->getTransformedVertices();
	const size_t verticesCount = vertices.size();

	{
		bool collisionSide = false;
		for (size_t i = 0; i < verticesCount; i++)
		{
			glm::vec2 va = vertices[i];
			glm::vec2 vb = vertices[(i + 1) % verticesCount];

			glm::vec2 edge = vb - va;
			glm::vec2 axis = glm::normalize(glm::vec2(-edge.y, edge.x));

			glm::vec2 rangeA = projectVertices(vertices, axis);
			glm::vec2 rangeB = projectCircle(circleA->position, circleA->radius, axis);

			if (rangeA.x >= rangeB.y || rangeB.x >= rangeA.y)
			{
				return false;
			}

			float bmax_amin = rangeB.y - rangeA.x;
			float amax_bmin = rangeA.y - rangeB.x;
			float axisDepth = fminf(bmax_amin, amax_bmin);
			if (axisDepth < depth)
			{
				depth = axisDepth;
				normal = axis;
				if (bmax_amin < amax_bmin)
				{
					collisionSide = true;
				}
				else
				{
					collisionSide = false;
				}
			}
		}

		glm::vec2 closestPoint = findClosestVertexOnPolygon(bodyA->position, vertices);
		glm::vec2 axis = glm::normalize(closestPoint - circleA->position);

		glm::vec2 rangeA = projectVertices(vertices, axis);
		glm::vec2 rangeB = projectCircle(circleA->position, circleA->radius, axis);

		if (rangeA.x >= rangeB.y || rangeB.x >= rangeA.y)
		{
			return false;
		}

		float bmax_amin = rangeB.y - rangeA.x;
		float amax_bmin = rangeA.y - rangeB.x;
		float axisDepth = fminf(bmax_amin, amax_bmin);
		if (axisDepth < depth)
		{
			depth = axisDepth;
			normal = axis;
			collisionSide = bmax_amin < amax_bmin;
		}

		if (collisionSide)
		{
			normal = -normal;
		}
	}

	// Find contact point
	// TODO: Maybe do that in the first loop
	glm::vec2 closestContact;
	{
		float minDistanceSquared = FLT_MAX;
		for (size_t i = 0; i < verticesCount; i++)
		{
			glm::vec2 va = vertices[i];
			glm::vec2 vb = vertices[(i + 1) % verticesCount];

			float distanceSquared;
			glm::vec2 contact = findClosestPointOnSegment(va, vb, circleA->position, distanceSquared);
		
			if (distanceSquared < minDistanceSquared)
			{
				minDistanceSquared = distanceSquared;
				closestContact = contact;
			}
		}
	}

	result.normal = -normal;
	result.depth = depth;
	result.contacts[0] = closestContact;
	result.countOfContacts = 1;
	return true;
}

void Collisions::checkCollision(std::unique_ptr<RigidBody>& bodyA, std::unique_ptr<RigidBody>& bodyB)
{
	checkCollision(bodyA.get(), bodyB.get());
}

void Collisions::checkCollision(RigidBody* bodyA_, RigidBody* bodyB_)
{
	CollisionManifold manifold;

	RigidBody* bodyA = bodyA_;
	RigidBody* bodyB = bodyB_;

	const bool swap = (size_t)bodyB->shapeType < (size_t)bodyA->shapeType;
	if (swap)
	{
		auto temp = bodyA;
		bodyA = bodyB;
		bodyB = temp;
	}

	auto func = checkCollisionFunctionsMatrix[(size_t)bodyA->shapeType][(size_t)bodyB->shapeType];
	bool colliding = func(manifold, bodyA, bodyB);
	if (!colliding)
	{
		return;
	}

	if (swap)
	{
		manifold.normal = -manifold.normal;
	}

	manifold.bodyA = bodyA_;
	manifold.bodyB = bodyB_;

	manifolds.push_back(manifold);
}

const std::vector<CollisionManifold>& Collisions::getManifolds()
{
	return manifolds;
}

bool Collisions::areAnyCollisionsFound()
{
	return manifolds.size() > 0;
}

void Collisions::clearManifolds()
{
	manifolds.clear();
}