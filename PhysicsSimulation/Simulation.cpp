#include "Simulation.h"

void Simulation::addCircle(const glm::vec2& position, float radius)
{
	circles.emplace_back(position, radius);
}

const std::vector<RigidCircle>& Simulation::getCircles() const
{
	return circles;
}
