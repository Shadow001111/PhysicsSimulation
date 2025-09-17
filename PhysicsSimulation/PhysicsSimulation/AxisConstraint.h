#pragma once
#include "BaseConstraint.h"

class AxisConstraint : public BaseConstraint
{
	bool disableX, disableY;
public:
    AxisConstraint(RigidBody* bodyA, RigidBody* bodyB, const glm::vec2& anchorA, const glm::vec2& anchorB, bool disableX, bool disableY);
    ~AxisConstraint() = default;

    // Copy and move semantics
    AxisConstraint(const AxisConstraint&) = delete;
    AxisConstraint& operator=(const AxisConstraint&) = delete;
    AxisConstraint(AxisConstraint&&) = default;
    AxisConstraint& operator=(AxisConstraint&&) = default;

    //
    void update(float deltaTime) override;
};

