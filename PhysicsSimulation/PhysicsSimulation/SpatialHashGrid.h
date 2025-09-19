#pragma once
#include "RigidBody/RigidBody.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

using RigidBodyPair = std::pair<RigidBody*, RigidBody*>;

// TODO: Maybe add move semantics
struct GridCell
{
    std::vector<RigidBody*> bodies;
};

struct GridCoordHash
{
    size_t operator()(const std::pair<int, int>& coord) const noexcept
    {
        size_t h1 = (size_t)coord.first;
        size_t h2 = (size_t)coord.second;
        return h1 | (h2 << 32);
    }
};

struct RigidBodyPairHash
{
    size_t operator()(const std::pair<RigidBody*, RigidBody*>& pair) const noexcept
    {
        size_t h1 = (size_t)pair.first;
        size_t h2 = (size_t)pair.second;
        return h1 ^ (h2 << 1);
    }
};

class SpatialHashGrid
{
private:
    float cellSize;
    AABB worldBounds;
    std::unordered_map<std::pair<int, int>, GridCell, GridCoordHash> grid;

    // Helper methods
    std::pair<int, int> worldToGrid(float x, float y) const;
    void getCellsForAABB(const AABB& aabb, std::vector<std::pair<int, int>>& cells) const;
    void addBodyToCells(RigidBody* body);

public:
    SpatialHashGrid(const AABB& worldBounds, float cellSize);

    void clear();
    void rebuild(std::vector<std::unique_ptr<RigidBody>>& bodies);
    void getPotentialCollisions(std::vector<RigidBodyPair>& pairs) const;

    // Visualization helpers
    void getAllCellBounds(std::vector<AABB>& bounds) const;
    void getActiveCellBounds(std::vector<AABB>& bounds) const;
};