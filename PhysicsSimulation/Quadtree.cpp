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

void QuadtreeNode::insert(std::unique_ptr<RigidBody>* body)
{
    //If we have children, try to insert
    if (children[0] != nullptr)
    {
        const AABB& bodyAABB = (*body)->getAABB();

        for (auto& child : children)
        {
            if (child->bounds.isIntersecting(bodyAABB))
            {
                child->insert(body);
            }
        }
        return;
    }

    // Store in this node
    objects.push_back(body);

    // If we exceed capacity and can still subdivide, do so
    if (objects.size() > MAX_OBJECTS && level < MAX_LEVELS)
    {
        subdivide();

        for (auto& object : objects)
        {
            const AABB& objectAABB = (*object)->getAABB();
            for (auto& child : children)
            {
                if (child->bounds.isIntersecting(objectAABB))
                {
                    child->insert(object);
                }
            }
        }

        objects.clear();
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
