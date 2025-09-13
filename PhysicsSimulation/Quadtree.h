#pragma once
#include "AABB.h"
#include "RigidBody.h"
#include <vector>
#include <memory>

class QuadtreeNode
{
    static constexpr int MAX_OBJECTS = 4;
    static constexpr int MAX_LEVELS = 8;

    AABB bounds;
    std::vector<std::unique_ptr<RigidBody>*> bodies;
    std::unique_ptr<QuadtreeNode> children[4];
    int level;
public:
    QuadtreeNode(const AABB& bounds, int level);

    void subdivide();
    void clear();

    int getQuadrant(const AABB& aabb) const;
    bool canFitInQuadrant(const AABB& aabb, int quadrant) const;
    
    void insert(std::unique_ptr<RigidBody>* body);
    void retrieve(std::vector<std::unique_ptr<RigidBody>*>& returnBodies, const AABB& searchAABB);

    void getAllBounds(std::vector<AABB>& bounds) const;
};

using RigidBodyPair = std::pair<std::unique_ptr<RigidBody>*, std::unique_ptr<RigidBody>*>;

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
