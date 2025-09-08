#include "Collisions.h"
#include "math.h"
#include <iostream>

glm::vec2 Collisions::projectVertices(const std::vector<glm::vec2>& vertices, glm::vec2 axis)
{
	float min = FLT_MAX;
	float max = FLT_MIN;

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