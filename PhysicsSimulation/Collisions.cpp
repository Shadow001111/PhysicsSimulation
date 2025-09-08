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

bool Collisions::circleCircle(RigidCircle& a, RigidCircle& b)
{
	glm::vec2 deltaPos = b.position - a.position;
	float distanceSquared = glm::dot(deltaPos, deltaPos);

	float radiusSum = a.radius + b.radius;
	if (distanceSquared >= radiusSum * radiusSum)
	{
		return false;
	}

	float distance = sqrtf(distanceSquared);
	glm::vec2 normal = deltaPos / distance;
	float displacement = radiusSum - distance;

	float bDisplacementRatio = a.mass / (a.mass + b.mass);
	float aDisplacementRatio = 1.0f - bDisplacementRatio;

	// Position
	a.position -= normal * (displacement * aDisplacementRatio);
	b.position += normal * (displacement * bDisplacementRatio);

	// Velocity
	float elasticity = 1.0f + fminf(a.elasticity, b.elasticity);

	glm::vec2 relVel = b.velocity - a.velocity;

	float velAlongNormal = glm::dot(relVel, normal);

	float j = elasticity * velAlongNormal / (a.invMass + b.invMass);

	a.velocity += (j * a.invMass) * normal;
	b.velocity -= (j * b.invMass) * normal;

	//
	a.transformUpdateRequired = true;
	b.transformUpdateRequired = true;
	return true;
}

bool Collisions::circleCircle(std::unique_ptr<RigidBody>& a, std::unique_ptr<RigidBody>& b)
{
	auto& a_ = *(dynamic_cast<RigidCircle*>(a.get()));
	auto& b_ = *(dynamic_cast<RigidCircle*>(b.get()));
	return circleCircle(a_, b_);
}

bool Collisions::polygonPolygon(RigidPolygon& a, RigidPolygon& b)
{
	const auto& vertsA = a.getTransformedVertices();
	const auto& vertsB = b.getTransformedVertices();

	const size_t countA = vertsA.size();
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
			return false;
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
			return false;
		}
	}

	return true;
}

bool Collisions::polygonPolygon(std::unique_ptr<RigidBody>& a, std::unique_ptr<RigidBody>& b)
{
	auto& a_ = *(dynamic_cast<RigidPolygon*>(a.get()));
	auto& b_ = *(dynamic_cast<RigidPolygon*>(b.get()));
	return polygonPolygon(a_, b_);
}