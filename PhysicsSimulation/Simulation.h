#pragma once
#include <vector>
#include <memory>

#include "Collisions.h"
#include "Quadtree.h"

class Simulation
{
	// Simulation parameters
	float fixedTimeStep = 1.0f / 300.0f;
	unsigned int iterationsToSolveCollisions = 5;
	unsigned int maxIterationsPerFrame = 9999999999999;// 32;

	float gravity = -9.81f;

	const float WORLD_BOUNDS = 3.0f;

	// Quadtree
	std::unique_ptr<Quadtree> quadtree;
	AABB worldBounds;
	bool useQuadtree = true; // Toggle for performance comparison

	//
	float accumulatedUpdateTime = 0.0;

	std::vector<std::unique_ptr<RigidBody>> bodies;

	void singlePhysicsStep();
	void updateOrientationAndVelocity();

	void detectCollisions();
	void detectCollisionsBruteForce();
	void detectCollisionsWithQuadtree();

	void resolveCollisionsSingleStep();
public:
	Simulation();

	// Bodies
	void addCircle(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, const Material& material, float radius);
	void addBox(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, const Material& material, const glm::vec2& size);
	void addPolygon(const glm::vec2& pos, const glm::vec2& vel, float rot, float angVel, float mass, float inertia, const Material& material, const std::vector<glm::vec2>& vertices);

	const std::vector<std::unique_ptr<RigidBody>>& getBodies() const;

	// Simulation
	int update(float deltaTime);

	//Quadtree
	void setUseQuadtree(bool enable);
	bool isUsingQuadtree() const;

	void getQuadtreeBounds(std::vector<AABB>& bounds) const;

	// Profiler
	void printPerfomanceReport() const;
};