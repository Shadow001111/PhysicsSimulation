#include "SpatialHashGrid.h"
#include "Profiler.h"
#include <algorithm>
#include <iostream>

SpatialHashGrid::SpatialHashGrid(const AABB& worldBounds, float cellSize)
    : worldBounds(worldBounds), cellSize(cellSize)
{
    // Reserve space for the hash map to reduce rehashing
    grid.reserve(1024);
}

std::pair<int, int> SpatialHashGrid::worldToGrid(float x, float y) const
{
    int gridX = static_cast<int>(std::floor(x / cellSize));
    int gridY = static_cast<int>(std::floor(y / cellSize));
    return std::make_pair(gridX, gridY);
}

void SpatialHashGrid::getCellsForAABB(const AABB& aabb, std::vector<std::pair<int, int>>& cells) const
{
    // Get grid coordinates for min and max corners
    auto minCell = worldToGrid(aabb.min.x, aabb.min.y);
    auto maxCell = worldToGrid(aabb.max.x, aabb.max.y);

    // Add all cells that the AABB spans
    for (int x = minCell.first; x <= maxCell.first; ++x)
    {
        for (int y = minCell.second; y <= maxCell.second; ++y)
        {
            cells.emplace_back(x, y);
        }
    }
}

void SpatialHashGrid::addBodyToCells(RigidBody* body)
{
    static std::vector<std::pair<int, int>> cells;
    cells.clear();

    getCellsForAABB(body->getAABB(), cells);

    for (const auto& cellCoord : cells)
    {
        grid[cellCoord].bodies.push_back(body);
    }
}

void SpatialHashGrid::clear()
{
    PROFILE_FUNCTION();

    // Clear all cells but keep the hash map structure
    // Remove empty cells from the hash map first
    auto it = grid.begin();
    while (it != grid.end())
    {
        if (it->second.bodies.empty())
        {
            it = grid.erase(it);
        }
        else
        {
            // Clear the cell but keep it in the map
            it->second.bodies.clear();
            ++it;
        }
    }
}

void SpatialHashGrid::rebuild(std::vector<std::unique_ptr<RigidBody>>& bodies)
{
    PROFILE_FUNCTION();

    clear();

    // Insert all bodies into the grid
    for (auto& body : bodies)
    {
        addBodyToCells(body.get());
    }
}

void SpatialHashGrid::getPotentialCollisions(std::vector<RigidBodyPair>& pairs) const
{
    PROFILE_FUNCTION();

    // Use a set to avoid duplicate pairs
    static std::unordered_set<RigidBodyPair, RigidBodyPairHash> uniquePairs;
    uniquePairs.clear();

    // Check each active cell for potential collisions
    for (const auto& cellPair : grid)
    {
        const GridCell& cell = cellPair.second;
        const auto& cellBodies = cell.bodies;

        size_t bodiesCount = cellBodies.size();
        if (bodiesCount < 2)
        {
            continue;
        }

        // Check all pairs within this cell
        for (size_t i = 0; i < bodiesCount - 1; i++)
        {
            RigidBody* bodyA = cellBodies[i];
            const AABB& bodyA_AABB = bodyA->getAABB_noUpdate();
            const bool isBodyAStatic = bodyA->isStatic();

            for (size_t j = i + 1; j < bodiesCount; j++)
            {
                RigidBody* bodyB = cellBodies[j];
                const bool isBodyBStatic = bodyB->isStatic();

                if (isBodyAStatic && isBodyBStatic)
                {
                    continue;
                }

                const AABB& bodyB_AABB = bodyB->getAABB_noUpdate();
                if (!bodyA_AABB.isIntersecting(bodyB_AABB))
                {
                    continue;
                }

                // Ensure consistent ordering to avoid duplicates
                RigidBodyPair pair = bodyA < bodyB ? std::make_pair(bodyA, bodyB) : std::make_pair(bodyB, bodyA);
                uniquePairs.insert(pair);
            }
        }
    }

    // Convert to vector
    pairs.clear();
    pairs.reserve(uniquePairs.size());
    for (const auto& pair : uniquePairs)
    {
        pairs.push_back(pair);
    }
}

void SpatialHashGrid::getAllCellBounds(std::vector<AABB>& bounds) const
{
    bounds.clear();

    // Calculate world bounds in grid coordinates
    auto minGridCoord = worldToGrid(worldBounds.min.x, worldBounds.min.y);
    auto maxGridCoord = worldToGrid(worldBounds.max.x, worldBounds.max.y);

    // Generate bounds for all possible cells in the world
    for (int x = minGridCoord.first; x <= maxGridCoord.first; ++x)
    {
        for (int y = minGridCoord.second; y <= maxGridCoord.second; ++y)
        {
            float minX = x * cellSize;
            float minY = y * cellSize;
            float maxX = minX + cellSize;
            float maxY = minY + cellSize;

            bounds.emplace_back(minX, minY, maxX, maxY);
        }
    }
}

void SpatialHashGrid::getActiveCellBounds(std::vector<AABB>& bounds) const
{
    bounds.clear();

    // Only return bounds for cells that contain objects
    for (const auto& pair : grid)
    {
        if (!pair.second.bodies.empty())
        {
            const auto& coord = pair.first;
            float minX = coord.first * cellSize;
            float minY = coord.second * cellSize;
            float maxX = minX + cellSize;
            float maxY = minY + cellSize;

            bounds.emplace_back(minX, minY, maxX, maxY);
        }
    }
}