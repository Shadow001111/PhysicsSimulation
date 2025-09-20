#pragma once
#include "BaseConstraint.h"

class AngularVelocityConstraint : public BaseConstraint
{
    float angularVelocity;
public:
	AngularVelocityConstraint(RigidBody* body, float angularVelocity);
	~AngularVelocityConstraint() = default;

    // Copy and move semantics
    AngularVelocityConstraint(const AngularVelocityConstraint&) = delete;
    AngularVelocityConstraint& operator=(const AngularVelocityConstraint&) = delete;
    AngularVelocityConstraint(AngularVelocityConstraint&&) = default;
    AngularVelocityConstraint& operator=(AngularVelocityConstraint&&) = default;

    //
    void update(float deltaTime) override;
};

