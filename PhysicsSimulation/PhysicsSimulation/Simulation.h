#pragma once
#include <vector>
#include <memory>

#include "Collisions.h"
#include "Quadtree.h"
#include "SpatialHashGrid.h"

enum class CollisionDetectionMethod : int
{
	BruteForce,
	Quadtree,
	SpatialHashGrid,
	_COUNT
};

class Simulation
{
	// Simulation parameters
	float fixedTimeStep = 1.0f / 300.0f;
	unsigned int iterationsToSolveCollisions = 10;
	unsigned int maxIterationsPerFrame = 32;

	float gravity = -9.81f;

	const float WORLD_BOUNDS = 3.0f;

	// Spatial data structures
	std::unique_ptr<Quadtree> quadtree;
	std::unique_ptr<SpatialHashGrid> spatialHashGrid;

	AABB worldBounds;

	CollisionDetectionMethod collisionMethod = CollisionDetectionMethod::SpatialHashGrid;

	//
	float accumulatedUpdateTime = 0.0;

	std::vector<std::unique_ptr<RigidBody>> bodies;

	void singlePhysicsStep();
	void updateOrientationAndVelocity();

	void detectCollisions();
	void detectCollisionsBruteForce();
	void detectCollisionsWithQuadtree();
	void detectCollisionsWithHashGrid();

	void resolveCollisionsSingleStep();
public:
	Simulation();

	// Bodies
	std::unique_ptr<RigidBody>& addCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, float radius);
	std::unique_ptr<RigidBody>& addBox(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, const glm::vec2& size);
	std::unique_ptr<RigidBody>& addPolygon(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, const std::vector<glm::vec2>& vertices);

	const std::vector<std::unique_ptr<RigidBody>>& getBodies() const;

	// Simulation
	int update(float deltaTime);

	// Collision detection method selection
	void setCollisionDetectionMethod(CollisionDetectionMethod method);
	CollisionDetectionMethod getCollisionDetectionMethod() const;

	// Quadtree
	void getQuadtreeBounds(std::vector<AABB>& bounds) const;

	// Spatial hash grid
	void getHashGridBounds(std::vector<AABB>& bounds, bool onlyActive) const;
	void setSpatialHashGridCellSize(float cellSize);

	// Profiler
	void printPerfomanceReport() const;
};