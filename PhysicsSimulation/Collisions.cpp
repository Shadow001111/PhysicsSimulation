#include "Collisions.h"
#include "math.h"
#include <iostream>

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
	float min, max;

	glm::vec2 dirAndR = axis * radius;

	glm::vec2 p1 = position + dirAndR;
	glm::vec2 p2 = position - dirAndR;

	min = glm::dot(p1, axis);
	max = glm::dot(p2, axis);
	if (max < min)
	{
		return { max, min };
	}
	return { min, max };
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

void Collisions::circleCircle(RigidCircle& a, RigidCircle& b, std::unique_ptr<RigidBody>* bodyA, std::unique_ptr<RigidBody>* bodyB)
{
	glm::vec2 deltaPos = b.position - a.position;
	float distanceSquared = glm::dot(deltaPos, deltaPos);

	float radiusSum = a.radius + b.radius;
	if (distanceSquared >= radiusSum * radiusSum)
	{
		return;
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

	glm::vec2 contact = a.position + normal * a.radius;

	manifolds.emplace_back(bodyA, bodyB, normal, depth, contact, glm::vec2(), 1);
}

void Collisions::polygonPolygon(RigidPolygon& a, RigidPolygon& b, std::unique_ptr<RigidBody>* bodyA, std::unique_ptr<RigidBody>* bodyB)
{
	glm::vec2 normal = {};
	float depth = FLT_MAX;

	const auto& vertsA = a.getTransformedVertices();
	const auto& vertsB = b.getTransformedVertices();

	const size_t countA = vertsA.size();
	bool collisionSide = false;
	for (size_t i = 0; i < countA; i++)
	{
		glm::vec2 va = vertsA[i];
		glm::vec2 vb = vertsA[(i + 1) % countA];

		glm::vec2 edge = vb - va;
		glm::vec2 axis = glm::normalize(glm::vec2(-edge.y, edge.x));

		glm::vec2 rangeA = projectVertices(vertsA, axis);
		glm::vec2 rangeB = projectVertices(vertsB, axis);

		if (rangeA.x >= rangeB.y || rangeB.x >= rangeA.y)
		{
			return;
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

	const size_t countB = vertsB.size();
	for (size_t i = 0; i < countB; i++)
	{
		glm::vec2 va = vertsB[i];
		glm::vec2 vb = vertsB[(i + 1) % countB];

		glm::vec2 edge = vb - va;
		glm::vec2 axis = glm::normalize(glm::vec2(-edge.y, edge.x));

		glm::vec2 rangeA = projectVertices(vertsA, axis);
		glm::vec2 rangeB = projectVertices(vertsB, axis);

		if (rangeA.x >= rangeB.y || rangeB.x >= rangeA.y)
		{
			return;
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

	if (collisionSide)
	{
		normal = -normal;
	}

	manifolds.emplace_back(bodyA, bodyB, normal, depth, glm::vec2(), glm::vec2(), 0);
}

void Collisions::circlePolygon(RigidCircle& a, RigidPolygon& b, std::unique_ptr<RigidBody>* bodyA, std::unique_ptr<RigidBody>* bodyB)
{
	glm::vec2 normal = {};
	float depth = FLT_MAX;

	const auto& verts = b.getTransformedVertices();
	const size_t count = verts.size();

	{
		bool collisionSide = false;
		for (size_t i = 0; i < count; i++)
		{
			glm::vec2 va = verts[i];
			glm::vec2 vb = verts[(i + 1) % count];

			glm::vec2 edge = vb - va;
			glm::vec2 axis = glm::normalize(glm::vec2(-edge.y, edge.x));

			glm::vec2 rangeA = projectVertices(verts, axis);
			glm::vec2 rangeB = projectCircle(a.position, a.radius, axis);

			if (rangeA.x >= rangeB.y || rangeB.x >= rangeA.y)
			{
				return;
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

		glm::vec2 closestPoint = findClosestVertexOnPolygon(a.position, verts);
		glm::vec2 axis = glm::normalize(closestPoint - a.position);

		glm::vec2 rangeA = projectVertices(verts, axis);
		glm::vec2 rangeB = projectCircle(a.position, a.radius, axis);

		if (rangeA.x >= rangeB.y || rangeB.x >= rangeA.y)
		{
			return;
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
		for (size_t i = 0; i < count; i++)
		{
			glm::vec2 va = verts[i];
			glm::vec2 vb = verts[(i + 1) % count];

			float distanceSquared;
			glm::vec2 contact = findClosestPointOnSegment(va, vb, a.position, distanceSquared);
		
			if (distanceSquared < minDistanceSquared)
			{
				minDistanceSquared = distanceSquared;
				closestContact = contact;
			}
		}
	}

	manifolds.emplace_back(bodyA, bodyB, -normal, depth, closestContact, glm::vec2(), 1);
}

void Collisions::checkCollision(std::unique_ptr<RigidBody>& bodyA, std::unique_ptr<RigidBody>& bodyB)
{
	if (bodyA->shapeType == ShapeType::Circle && bodyB->shapeType == ShapeType::Circle)
	{
		auto& a = *(dynamic_cast<RigidCircle*>(bodyA.get()));
		auto& b = *(dynamic_cast<RigidCircle*>(bodyB.get()));
		circleCircle(a, b, &bodyA, &bodyB);
	}
	else if (bodyA->shapeType == ShapeType::Polygon && bodyB->shapeType == ShapeType::Polygon)
	{
		auto& a = *(dynamic_cast<RigidPolygon*>(bodyA.get()));
		auto& b = *(dynamic_cast<RigidPolygon*>(bodyB.get()));
		polygonPolygon(a, b, &bodyA, &bodyB);
	}
	else if (bodyA->shapeType == ShapeType::Circle && bodyB->shapeType == ShapeType::Polygon)
	{
		auto& a = *(dynamic_cast<RigidCircle*>(bodyA.get()));
		auto& b = *(dynamic_cast<RigidPolygon*>(bodyB.get()));
		circlePolygon(a, b, &bodyA, &bodyB);
	}
	else if (bodyA->shapeType == ShapeType::Polygon && bodyB->shapeType == ShapeType::Circle)
	{
		auto& a = *(dynamic_cast<RigidCircle*>(bodyB.get()));
		auto& b = *(dynamic_cast<RigidPolygon*>(bodyA.get()));
		circlePolygon(a, b, &bodyB, &bodyA);
	}
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

CollisionManifold::CollisionManifold(std::unique_ptr<RigidBody>* a, std::unique_ptr<RigidBody>* b, const glm::vec2& n, float d, const glm::vec2& contact1, const glm::vec2& contact2, int countOfContacts)
	: bodyA(a), bodyB(b), normal(n), depth(d), countOfContacts(countOfContacts)
{
	contacts[0] = contact1;
	contacts[1] = contact2;
}
