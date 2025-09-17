#pragma once
#include "RigidBody.h"

enum class ConstraintType : unsigned int
{
    Spring,
    Axis,
};

class BaseConstraint
{
protected:
    RigidBody* bodyA;
    RigidBody* bodyB;
    ConstraintType type;

    glm::vec2 localAnchorA;
    glm::vec2 localAnchorB;

    static glm::vec2 rotatePoint(const glm::vec2& point, float angle);
public:
    BaseConstraint(RigidBody* bodyA, RigidBody* bodyB, ConstraintType type, const glm::vec2& anchorA, const glm::vec2& anchorB);
    ~BaseConstraint() = default;

    // Copy and move semantics
    BaseConstraint(const BaseConstraint&) = delete;
    BaseConstraint& operator=(const BaseConstraint&) = delete;
    BaseConstraint(BaseConstraint&&) = default;
    BaseConstraint& operator=(BaseConstraint&&) = default;

    //
    virtual void update(float deltaTime) = 0;
};

