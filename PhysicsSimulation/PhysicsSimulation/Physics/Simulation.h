#pragma once
#include <vector>
#include <memory>

#include "Collision/Collisions.h"

#include "Spatial/Quadtree.h"
#include "Spatial/SpatialHashGrid.h"

#include "Constraints/BaseConstraint.h"
#include "Constraints/SpringConstraint.h"
#include "Constraints/AxisConstraint.h"
#include "Constraints/AngularVelocityConstraint.h"

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
	unsigned int iterationsToSolveCollisions = 8;
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
	std::vector<std::unique_ptr<BaseConstraint>> constraints;

	void singlePhysicsStep();
	void updateOrientationAndVelocity();
	void updateConstraints();

	void detectCollisions();
	void detectCollisionsBruteForce();
	void detectCollisionsWithQuadtree();
	void detectCollisionsWithHashGrid();

	void resolveCollisionsSingleStep();
public:
	Simulation();

	// Bodies
	RigidBody* addCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, float radius);
	RigidBody* addBox(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, const glm::vec2& size);
	RigidBody* addPolygon(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, Material* material, const std::vector<glm::vec2>& vertices);

	const std::vector<std::unique_ptr<RigidBody>>& getBodies() const;

	// Constraints
	BaseConstraint* addSpringConstraint(RigidBody* bodyA, RigidBody* bodyB, const glm::vec2& anchorA, const glm::vec2& anchorB, float distance, float stiffness);
	BaseConstraint* addAxisConstraint(RigidBody* body, bool disableX, bool disableY);
	BaseConstraint* addAngularVelocityConstraint(RigidBody* body, float angularVelocity);

	const std::vector<std::unique_ptr<BaseConstraint>>& getConstraints() const;

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