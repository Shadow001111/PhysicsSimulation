#pragma once
#include "BaseConstraint.h"

class SpringConstraint : public BaseConstraint
{
    float distanceParam;
    float stiffnessParam;
public:
	SpringConstraint(RigidBody* bodyA, RigidBody* bodyB, const glm::vec2& anchorA, const glm::vec2& anchorB, float distance, float stiffness);
	~SpringConstraint() = default;

    // Copy and move semantics
    SpringConstraint(const SpringConstraint&) = delete;
    SpringConstraint& operator=(const SpringConstraint&) = delete;
    SpringConstraint(SpringConstraint&&) = default;
    SpringConstraint& operator=(SpringConstraint&&) = default;

    //
    void update(float deltaTime) override;
};

