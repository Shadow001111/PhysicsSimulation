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

bool AABB::isIntersecting(const AABB& other) const
{
    return (min.x <= other.max.x && max.x >= other.min.x) &&
           (min.y <= other.max.y && max.y >= other.min.y);
}
