#include "Collisions.h"
#include "math.h"
#include <iostream>

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

glm::vec2 Collisions::findClosestPointOnPolygon(const glm::vec2& point, const std::vector<glm::vec2>& vertices)
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

CollisionInfo Collisions::circleCircle(RigidCircle& a, RigidCircle& b)
{
	glm::vec2 deltaPos = b.position - a.position;
	float distanceSquared = glm::dot(deltaPos, deltaPos);

	float radiusSum = a.radius + b.radius;
	if (distanceSquared >= radiusSum * radiusSum)
	{
		return {};
	}

	float distance = sqrtf(distanceSquared);
	glm::vec2 normal = deltaPos / distance;
	float depth = radiusSum - distance;
	return {normal, depth};
}

CollisionInfo Collisions::circleCircle(std::unique_ptr<RigidBody>& a, std::unique_ptr<RigidBody>& b)
{
	auto& a_ = *(dynamic_cast<RigidCircle*>(a.get()));
	auto& b_ = *(dynamic_cast<RigidCircle*>(b.get()));
	return circleCircle(a_, b_);
}

CollisionInfo Collisions::polygonPolygon(RigidPolygon& a, RigidPolygon& b)
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
			return {};
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
		glm::vec2 vb = vertsB[(i + 1) % countA];

		glm::vec2 edge = vb - va;
		glm::vec2 axis = glm::normalize(glm::vec2(-edge.y, edge.x));

		glm::vec2 rangeA = projectVertices(vertsA, axis);
		glm::vec2 rangeB = projectVertices(vertsB, axis);

		if (rangeA.x >= rangeB.y || rangeB.x >= rangeA.y)
		{
			return {};
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

	return {normal, depth};
}

CollisionInfo Collisions::polygonPolygon(std::unique_ptr<RigidBody>& a, std::unique_ptr<RigidBody>& b)
{
	auto& a_ = *(dynamic_cast<RigidPolygon*>(a.get()));
	auto& b_ = *(dynamic_cast<RigidPolygon*>(b.get()));
	return polygonPolygon(a_, b_);
}

CollisionInfo Collisions::circlePolygon(RigidCircle& a, RigidPolygon& b)
{
	glm::vec2 normal = {};
	float depth = FLT_MAX;

	const auto& verts = b.getTransformedVertices();

	const size_t count = verts.size();
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
			return {};
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

	glm::vec2 closestPoint = findClosestPointOnPolygon(a.position, verts);
	glm::vec2 axis = glm::normalize(closestPoint - a.position);

	glm::vec2 rangeA = projectVertices(verts, axis);
	glm::vec2 rangeB = projectCircle(a.position, a.radius, axis);

	if (rangeA.x >= rangeB.y || rangeB.x >= rangeA.y)
	{
		return {};
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

	return { -normal, depth };
}

CollisionInfo Collisions::circlePolygon(std::unique_ptr<RigidBody>& a, std::unique_ptr<RigidBody>& b)
{
	auto& a_ = *(dynamic_cast<RigidCircle*>(a.get()));
	auto& b_ = *(dynamic_cast<RigidPolygon*>(b.get()));
	return circlePolygon(a_, b_);
}
