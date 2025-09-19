#pragma once
#include "RigidBody/RigidBody.h"
#include <vector>
#include <memory>
#include <stack>

class QuadtreeNode
{
    static constexpr int MAX_OBJECTS = 4;
    static constexpr int MAX_LEVELS = 8;

    AABB bounds;
    std::vector<RigidBody*> bodies;
    std::unique_ptr<QuadtreeNode> children[4];
    int level;

    // Object pool static members
    static std::stack<std::unique_ptr<QuadtreeNode>> nodePool;
    static size_t totalNodesCreated;
public:
    QuadtreeNode(const AABB& bounds, int level);

    void subdivide();
    void clear();

    int getQuadrant(const AABB& aabb) const;
    bool canFitInQuadrant(const AABB& aabb, int quadrant) const;
    
    void insert(RigidBody* body);
    void retrieve(std::vector<RigidBody*>& returnBodies, const AABB& searchAABB);

    void getAllBounds(std::vector<AABB>& bounds) const;

    // Object pool methods
    static std::unique_ptr<QuadtreeNode> acquireNode(const AABB& bounds, int level);
    static void releaseNode(std::unique_ptr<QuadtreeNode> node);
    static void clearPool();
    static void preAllocatePool(size_t count);
    static size_t getPoolSize();
    static size_t getTotalNodesCreated();
private:
    void reset(const AABB& newBounds, int newLevel);
};

using RigidBodyPair = std::pair<RigidBody*, RigidBody*>;

class Quadtree
{
private:
    std::unique_ptr<QuadtreeNode> root;
    AABB worldBounds;
public:
    Quadtree(const AABB& worldBounds);

    void clear();
    void rebuild(std::vector<std::unique_ptr<RigidBody>>& bodies);
    void getPotentialCollisions(std::vector<RigidBodyPair>& pairs) const;

    void getAllBounds(std::vector<AABB>& bounds) const;
};
