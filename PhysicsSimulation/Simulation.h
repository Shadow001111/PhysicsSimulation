#pragma once
#include <vector>

#include "RigidCircle.h"

class Simulation
{
	std::vector<RigidCircle> circles;
public:
	Simulation() = default;

	void addCircle(const glm::vec2& position, float radius);

	const std::vector<RigidCircle>& getCircles() const;
};

