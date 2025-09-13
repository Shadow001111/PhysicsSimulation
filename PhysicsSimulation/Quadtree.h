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
    std::vector<std::unique_ptr<RigidBody>*> objects;
    std::unique_ptr<QuadtreeNode> children[4];
    int level;
public:
    QuadtreeNode(const AABB& bounds, int level);

    void subdivide();
    void clear();
    void insert(std::unique_ptr<RigidBody>* body);
    void detectCollisions() const;

    void getAllBounds(std::vector<AABB>& bounds) const;
};

class Quadtree
{
private:
    std::unique_ptr<QuadtreeNode> root;
    AABB worldBounds;
public:
    Quadtree(const AABB& worldBounds);

    void clear();
    void rebuild(std::vector<std::unique_ptr<RigidBody>>& bodies);
    void detectCollisions() const;

    void getAllBounds(std::vector<AABB>& bounds) const;
};
