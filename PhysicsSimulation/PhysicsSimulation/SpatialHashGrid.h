#pragma once
#include "AABB.h"
#include "RigidBody.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

using RigidBodyPair = std::pair<RigidBody*, RigidBody*>;

struct GridCell
{
    std::vector<RigidBody*> bodies;

    void clear() { bodies.clear(); }
    void addBody(RigidBody* body) { bodies.push_back(body); }
    size_t size() const { return bodies.size(); }
};

struct PairHash
{
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const noexcept
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1); // простая комбинация
    }
};

class SpatialHashGrid
{
private:
    float cellSize;
    AABB worldBounds;
    std::unordered_map<std::pair<int, int>, GridCell, PairHash> grid;

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