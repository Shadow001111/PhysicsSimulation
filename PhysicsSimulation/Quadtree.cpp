#include "Quadtree.h"
#include "Collisions.h"
#include "Profiler.h"

#include <algorithm>
#include <unordered_set>
#include <iostream>

struct PairHash
{
    template <class T1, class T2>
    std::size_t operator()(std::pair<T1, T2> const& p) const noexcept
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1); // простая комбинация
    }
};


Quadtree::Quadtree(const AABB& worldBounds) : worldBounds(worldBounds)
{
    QuadtreeNode::preAllocatePool(256 + 1);
    root = QuadtreeNode::acquireNode(worldBounds, 0);
}

void Quadtree::clear()
{
    PROFILE_FUNCTION();

    root->clear();
}

void Quadtree::rebuild(std::vector<std::unique_ptr<RigidBody>>& bodies)
{
    PROFILE_FUNCTION();

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

void Quadtree::getPotentialCollisions(std::vector<RigidBodyPair>& pairs) const
{
    PROFILE_FUNCTION();

    static std::vector<std::unique_ptr<RigidBody>*> allBodies;
    allBodies.clear();
    root->retrieve(allBodies, worldBounds);

    // Use hash set to avoid duplicate pairs
    static std::unordered_set<RigidBodyPair, PairHash> uniquePairs;
    uniquePairs.clear();

    // For each object, find all potential collision candidates
    static std::vector<std::unique_ptr<RigidBody>*> candidates;
    for (size_t i = 0; i < allBodies.size(); i++)
    {
        std::unique_ptr<RigidBody>* bodyA = allBodies[i];
        const AABB& bodyA_AABB = (*bodyA)->getAABB();
        const bool isBodyAStatic = (*bodyA)->isStatic();

        candidates.clear();
        root->retrieve(candidates, bodyA_AABB);

        for (auto* bodyB : candidates)
        {
            if (bodyA == bodyB)
            {
                continue;
            }

            if (isBodyAStatic && (*bodyB)->isStatic())
            {
                continue;
            }

            if (!bodyA_AABB.isIntersecting((*bodyB)->getAABB()))
            {
                continue;
            }

            // Ensure consistent ordering to avoid duplicates
            RigidBodyPair pair = bodyA < bodyB ? std::make_pair(bodyA, bodyB) : std::make_pair(bodyB, bodyA);
            uniquePairs.insert(pair);
            // TODO: Maybe add all pairts to vector and the sort it once and remove duplicates
        }
    }

    // Convert to vector
    pairs.reserve(uniquePairs.size());
    for (const auto& pair : uniquePairs)
    {
        pairs.push_back(pair);
    }
}

void Quadtree::getAllBounds(std::vector<AABB>& bounds) const
{
    root->getAllBounds(bounds);
}


std::stack<std::unique_ptr<QuadtreeNode>> QuadtreeNode::nodePool;
size_t QuadtreeNode::totalNodesCreated = 0;

QuadtreeNode::QuadtreeNode(const AABB& bounds, int level)
    : bounds(bounds), level(level)
{
    bodies.reserve(MAX_OBJECTS);
    for (int i = 0; i < 4; i++)
    {
        children[i] = nullptr;
    }
}

void QuadtreeNode::subdivide()
{
    // Create child bounds
    float halfWidth = (bounds.max.x - bounds.min.x) * 0.5f;
    float halfHeight = (bounds.max.y - bounds.min.y) * 0.5f;
    float centerX = bounds.min.x + halfWidth;
    float centerY = bounds.min.y + halfHeight;

    AABB childBounds[4] =
    {
        {{centerX, centerY}, bounds.max},
        {{bounds.min.x, centerY}, {centerX, bounds.max.y}},
        {bounds.min, {centerX, centerY}},
        {{centerX, bounds.min.y}, {bounds.max.x, centerY}},
    };

    for (int i = 0; i < 4; i++)
    {
        children[i] = acquireNode(childBounds[i], level + 1);
    }
}

void QuadtreeNode::clear()
{
    bodies.clear();
    for (auto& child : children)
    {
        if (child)
        {
            releaseNode(std::move(child));
        }
    }
}

int QuadtreeNode::getQuadrant(const AABB& aabb) const
{
    float midX = (bounds.max.x + bounds.min.x) * 0.5f;
    float midY = (bounds.max.y + bounds.min.y) * 0.5f;

    if (aabb.min.x >= midX)
    {
        if (aabb.min.y >= midY) return 0; // NE
        if (aabb.max.y <= midY) return 3; // SE
    }
    else if (aabb.max.x <= midX)
    {
        if (aabb.min.y >= midY) return 1; // NW
        if (aabb.max.y <= midY) return 2; // SW
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
    bodies.push_back(body);

    // If we exceed capacity and can still subdivide, do so
    if (bodies.size() > MAX_OBJECTS && level < MAX_LEVELS && children[0] == nullptr)
    {
        subdivide();

        // Try to move objects to children
        auto it = bodies.begin();
        while (it != bodies.end())
        {
            auto body_ = *it;
            const AABB& objAABB = (*body_)->getAABB();
            int quadrant = getQuadrant(objAABB);

            if (quadrant >= 0 && canFitInQuadrant(objAABB, quadrant))
            {
                children[quadrant]->insert(body_);
                it = bodies.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

void QuadtreeNode::retrieve(std::vector<std::unique_ptr<RigidBody>*>& returnBodies, const AABB& searchAABB)
{
    // Add objects from this node
    returnBodies.insert(returnBodies.end(), bodies.begin(), bodies.end());

    // Recursively check children if they exist and intersect
    if (children[0] != nullptr)
    {
        for (const auto& child : children)
        {
            if (child->bounds.isIntersecting(searchAABB))
            {
                child->retrieve(returnBodies, searchAABB);
            }
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

std::unique_ptr<QuadtreeNode> QuadtreeNode::acquireNode(const AABB& bounds, int level)
{
    std::unique_ptr<QuadtreeNode> node;

    if (!nodePool.empty())
    {
        node = std::move(const_cast<std::unique_ptr<QuadtreeNode>&>(nodePool.top()));
        nodePool.pop();
        node->reset(bounds, level);
    }
    else
    {
        node = std::make_unique<QuadtreeNode>(bounds, level);
        totalNodesCreated++;
    }

    return node;
}

void QuadtreeNode::releaseNode(std::unique_ptr<QuadtreeNode> node)
{
    if (node)
    {
        node->clear();
        nodePool.push(std::move(node));
    }
}

void QuadtreeNode::clearPool()
{
    while (!nodePool.empty())
    {
        nodePool.pop();
    }
}

void QuadtreeNode::preAllocatePool(size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        auto node = std::make_unique<QuadtreeNode>(AABB{}, 0);
        nodePool.push(std::move(node));
    }
    totalNodesCreated += count;
}

size_t QuadtreeNode::getPoolSize()
{
    return nodePool.size();
}

size_t QuadtreeNode::getTotalNodesCreated()
{
    return totalNodesCreated;
}

void QuadtreeNode::reset(const AABB& newBounds, int newLevel)
{
    bounds = newBounds;
    level = newLevel;
    bodies.clear();
    for (auto& child : children)
    {
        child.reset();
    }
}
