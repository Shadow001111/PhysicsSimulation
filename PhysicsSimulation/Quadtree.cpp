#include "Quadtree.h"
#include "Collisions.h"

#include <algorithm>
#include <unordered_set>
#include <iostream>

// Quadtree Implementation
Quadtree::Quadtree(const AABB& worldBounds) : worldBounds(worldBounds)
{
    root = std::make_unique<QuadtreeNode>(worldBounds, 0);
}

void Quadtree::clear()
{
    root->clear();
}

void Quadtree::rebuild(std::vector<std::unique_ptr<RigidBody>>& bodies)
{
    clear();
    for (auto& body : bodies)
    {
        // Only insert bodies that are within world bounds
        if (worldBounds.isIntersecting(body->getAABB()))
        {
            root->insert(&body);
        }
    }
}

void Quadtree::detectCollisions() const
{
    return;
    root->detectCollisions();
}

void Quadtree::getAllBounds(std::vector<AABB>& bounds) const
{
    root->getAllBounds(bounds);
}


QuadtreeNode::QuadtreeNode(const AABB& bounds, int level)
    : bounds(bounds), level(level)
{
    objects.reserve(MAX_OBJECTS);
    for (int i = 0; i < 4; i++)
    {
        children[i] = nullptr;
    }
}

void QuadtreeNode::subdivide()
{
    if (children[0] != nullptr) return; // Already subdivided

    float halfWidth = (bounds.max.x - bounds.min.x) * 0.5f;
    float halfHeight = (bounds.max.y - bounds.min.y) * 0.5f;
    float midX = bounds.min.x + halfWidth;
    float midY = bounds.min.y + halfHeight;

    children[0] = std::make_unique<QuadtreeNode>(AABB(midX, midY, bounds.max.x, bounds.max.y), level + 1);
    children[1] = std::make_unique<QuadtreeNode>(AABB(bounds.min.x, midY, midX, bounds.max.y), level + 1);
    children[2] = std::make_unique<QuadtreeNode>(AABB(bounds.min.x, bounds.min.y, midX, midY), level + 1);
    children[3] = std::make_unique<QuadtreeNode>(AABB(midX, bounds.min.y, bounds.max.x, midY), level + 1);
}

void QuadtreeNode::clear()
{
    objects.clear();
    for (int i = 0; i < 4; i++)
    {
        if (children[i])
        {
            children[i]->clear();
            children[i] = nullptr;
        }
    }
}

int QuadtreeNode::getQuadrant(const AABB& aabb) const
{
    float midX = bounds.min.x + (bounds.max.x - bounds.min.x) * 0.5f;
    float midY = bounds.min.y + (bounds.max.y - bounds.min.y) * 0.5f;

    bool fitsInTop = aabb.min.y >= midY;
    bool fitsInBottom = aabb.max.y <= midY;
    bool fitsInLeft = aabb.max.x <= midX;
    bool fitsInRight = aabb.min.x >= midX;

    if (fitsInRight)
    {
        if (fitsInTop) return 0; // NE
        if (fitsInBottom) return 3; // SE
    }
    else if (fitsInLeft)
    {
        if (fitsInTop) return 1; // NW
        if (fitsInBottom) return 2; // SW
    }

    return -1; // Doesn't fit cleanly in any quadrant
}

bool QuadtreeNode::canFitInQuadrant(const AABB& aabb, int quadrant) const
{
    if (quadrant < 0 || quadrant >= 4 || children[quadrant] == nullptr)
    {
        return false;
    }

    const AABB& childBounds = children[quadrant]->bounds;
    return
        aabb.min.x >= childBounds.min.x && aabb.max.x <= childBounds.max.x &&
        aabb.min.y >= childBounds.min.y && aabb.max.y <= childBounds.max.y;
}

void QuadtreeNode::insert(std::unique_ptr<RigidBody>* body)
{
    //If we have children, try to insert
    if (children[0] != nullptr)
    {
        const AABB& bodyAABB = (*body)->getAABB();
        int quadrant = getQuadrant(bodyAABB);
        if (canFitInQuadrant(bodyAABB, quadrant))
        {
            children[quadrant]->insert(body);
            return;
        }
    }

    // Store in this node
    objects.push_back(body);

    // If we exceed capacity and can still subdivide, do so
    if (objects.size() > MAX_OBJECTS && level < MAX_LEVELS && children[0] == nullptr)
    {
        subdivide();

        // Try to move objects to children
        auto it = objects.begin();
        while (it != objects.end())
        {
            const AABB& objAABB = (**it)->getAABB();
            int quadrant = getQuadrant(objAABB);

            if (quadrant >= 0 && canFitInQuadrant(objAABB, quadrant))
            {
                children[quadrant]->insert(*it);
                it = objects.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

void QuadtreeNode::detectCollisions() const
{
    if (children[0] != nullptr)
    {
        for (auto& child : children)
        {
            child->detectCollisions();
        }
        return;
    }

    size_t count = objects.size();
    if (count == 0)
    {
        return;
    }
    for (size_t index1 = 0; index1 < count - 1; index1++)
    {
        auto& body1 = *objects[index1];
        const AABB& aabb1 = body1->getAABB();
        const bool isBody1Static = body1->isStatic();
        for (size_t index2 = index1 + 1; index2 < count; index2++)
        {
            auto& body2 = *objects[index2];
            const bool isBody2Static = body2->isStatic();

            if (isBody1Static && isBody2Static)
            {
                continue;
            }

            const AABB& aabb2 = body2->getAABB();
            if (!aabb1.isIntersecting(aabb2))
            {
                continue;
            }

            Collisions::checkCollision(body1, body2);
        }
    }
}

void QuadtreeNode::getAllBounds(std::vector<AABB>& bounds) const
{
    bounds.push_back(this->bounds);

    if (children[0] != nullptr)
    {
        for (int i = 0; i < 4; i++)
        {
            children[i]->getAllBounds(bounds);
        }
    }
}
