#include "RigidPolygon.h"

#include "Core/Transform.h"
#include "Core/CoreMath.h"

#include <iostream>

void RigidPolygon::updateTransformedVertices() const
{
	Transform transform(position, rotation);

	size_t count = vertices.size();
	for (size_t i = 0; i < count; i++)
	{
		transformedVertices[i] = transform.transform(vertices[i] - localCenterOfMass) + localCenterOfMass;
	}
}

void RigidPolygon::updateAABB() const
{
	float minX = FLT_MAX, minY = FLT_MAX;
	float maxX = -FLT_MAX, maxY = -FLT_MAX;

	const auto& verts = getTransformedVertices();
	for (const auto& vert : verts)
	{
		minX = fminf(minX, vert.x);
		minY = fminf(minY, vert.y);

		maxX = fmaxf(maxX, vert.x);
		maxY = fmaxf(maxY, vert.y);
	}

	aabb.min = { minX, minY };
	aabb.max = { maxX, maxY };
}

RigidPolygon::RigidPolygon(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, const std::vector<glm::vec2>& verts)
	: RigidBody(pos, vel, rot, angVel, mass, inertia, material, ShapeType::Polygon), vertices(verts)
{
	transformedVertices.resize(vertices.size());
}

RigidPolygon::RigidPolygon(RigidPolygon&& other) noexcept
	: RigidBody(std::move(other)),
	vertices(std::move(other.vertices)),
	transformedVertices(std::move(other.transformedVertices))
{
}

RigidPolygon& RigidPolygon::operator=(RigidPolygon&& other) noexcept
{
	if (this != &other)
	{
		RigidBody::operator=(std::move(dynamic_cast<RigidBody&&>(other)));
		vertices = std::move(other.vertices);
		transformedVertices = std::move(other.transformedVertices);
	}
	return *this;
}

void RigidPolygon::move(const glm::vec2& shift)
{
	position += shift;
	aabbUpdateRequired = true;
	transformUpdateRequired = true;
}

void RigidPolygon::rotate(float angle)
{
	rotation += angle;
	aabbUpdateRequired = true;
	transformUpdateRequired = true;
}

void RigidPolygon::moveAndRotate(const glm::vec2& shift, float angle)
{
	position += shift;
	rotation += angle;
	aabbUpdateRequired = true;
	transformUpdateRequired = true;
}

BodyProperties RigidPolygon::calculateProperties(float density) const
{
	BodyProperties properties;

	float area = 0.0f;
	float inertia = 0.0f;
	glm::vec2 centerOfMass = {};

	{
		// Area and centroid
		size_t n = vertices.size();
		for (size_t i = 0; i < n; i++)
		{
			const glm::vec2& a = vertices[i];
			const glm::vec2& b = vertices[(i + 1) % n];
			float cross = CoreMath::cross(a, b);
			area += cross;
			centerOfMass += (a + b) * cross;
		}
		area = fabsf(area) * 0.5f;
		centerOfMass /= 6.0f * area;

		// Moment of inertia
		for (size_t i = 0; i < n; i++)
		{
			glm::vec2 a = vertices[i] - centerOfMass;
			glm::vec2 b = vertices[(i + 1) % n] - centerOfMass;
			inertia += CoreMath::cross(a, b) * (glm::dot(a, a) + glm::dot(a, b) + glm::dot(b, b));
		}
		inertia = fabsf(inertia) * density / 12.0f;
	}

	float mass = area * density;

	properties.mass = mass;
	properties.inertia = inertia;
	properties.centerOfMass = centerOfMass;
	return properties;
}

const std::vector<glm::vec2>& RigidPolygon::getVertices() const
{
	return vertices;
}

const std::vector<glm::vec2>& RigidPolygon::getTransformedVertices() const
{
	if (transformUpdateRequired)
	{
		transformUpdateRequired = false;
		updateTransformedVertices();
	}
	return transformedVertices;
}
