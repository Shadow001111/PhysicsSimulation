#include "AABB.h"

AABB::AABB() : min(), max()
{
}

AABB::AABB(const glm::vec2& min, const glm::vec2& max) : min(min), max(max)
{
}

AABB::AABB(float minX, float minY, float maxX, float maxY) : min(minX, minY), max(maxX, maxY)
{
}
