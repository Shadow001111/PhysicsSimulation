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
    root = std::make_unique<QuadtreeNode>(worldBounds, 0);
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
    std::unordered_set<RigidBodyPair, PairHash> uniquePairs;

    // For each object, find all potential collision candidates
    std::vector<std::unique_ptr<RigidBody>*> candidates;
    for (size_t i = 0; i < allBodies.size(); i++)
    {
        std::unique_ptr<RigidBody>* bodyA = allBodies[i];
        const AABB& bodyAABB = (*bodyA)->getAABB();

        candidates.clear();
        {
            PROFILE_SCOPE("Quadtree Single Body Retrieval");
            root->retrieve(candidates, bodyAABB);
        }

        {
            PROFILE_SCOPE("Quadtree Create Pairs");

            for (auto* bodyB : candidates)
            {
                if (bodyA != bodyB)
                {
                    // Ensure consistent ordering to avoid duplicates
                    RigidBodyPair pair = bodyA < bodyB ? std::make_pair(bodyA, bodyB) : std::make_pair(bodyB, bodyA);

                    // Check if both bodies can collide (at least one non-static)
                    if (!(*bodyA)->isStatic() || !(*bodyB)->isStatic())
                    {
                        uniquePairs.insert(pair);
                    }
                }
            }
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
    PROFILE_FUNCTION();

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
    PROFILE_FUNCTION();

    bodies.clear();
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
    PROFILE_FUNCTION();

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
    PROFILE_FUNCTION();

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
        PROFILE_SCOPE("QuadtreeNode Insert Into Children");

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
        PROFILE_SCOPE("QuadtreeNode Subdivide Trigger");

        subdivide();

        // Try to move objects to children
        {
            PROFILE_SCOPE("QuadtreeNode Redistribute Bodies");

            auto it = bodies.begin();
            while (it != bodies.end())
            {
                const AABB& objAABB = (**it)->getAABB();
                int quadrant = getQuadrant(objAABB);

                if (quadrant >= 0 && canFitInQuadrant(objAABB, quadrant))
                {
                    children[quadrant]->insert(*it);
                    it = bodies.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
    }
}

void QuadtreeNode::retrieve(std::vector<std::unique_ptr<RigidBody>*>& returnBodies, const AABB& searchAABB)
{
    PROFILE_FUNCTION();

    // Add objects from this node
    returnBodies.insert(returnBodies.end(), bodies.begin(), bodies.end());

    // Recursively check children if they exist and intersect
    if (children[0] != nullptr)
    {
        for (int i = 0; i < 4; i++)
        {
            if (children[i]->bounds.isIntersecting(searchAABB))
            {
                children[i]->retrieve(returnBodies, searchAABB);
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
