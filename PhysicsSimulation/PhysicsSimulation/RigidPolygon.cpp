#include "RigidPolygon.h"
#include "Transform.h"
#include <iostream>

void RigidPolygon::updateTransformedVertices()
{
	Transform transform(position, rotation);

	size_t count = vertices.size();
	for (size_t i = 0; i < count; i++)
	{
		transformedVertices[i] = transform.transform(vertices[i]);
	}
}

void RigidPolygon::updateAABB()
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

const std::vector<glm::vec2>& RigidPolygon::getVertices()
{
	return vertices;
}

const std::vector<glm::vec2>& RigidPolygon::getTransformedVertices()
{
	if (transformUpdateRequired)
	{
		transformUpdateRequired = false;
		updateTransformedVertices();
	}
	return transformedVertices;
}
