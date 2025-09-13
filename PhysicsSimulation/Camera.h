#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "AABB.h"

class Camera
{
    void updateViewMatrix();
    void updateProjectionMatrix();
    void updateAABB();
public:
    Camera(const glm::vec2& position = glm::vec2(0.0f), float zoom = 1.0f);

    // Getters
    const glm::vec2& getPosition() const;
    float getZoom() const;
    const glm::mat4& getViewMatrix();
    const glm::mat4& getProjectionMatrix();
    const AABB& getAABB();

    // Setters
    void setPosition(const glm::vec2& newPosition);
    void setZoom(float newZoom);
    void move(const glm::vec2& offset);
    void zoomBy(float factor);
    void setAspectRatio(float aspectRatio);

    // Screen to world coordinate conversion
    glm::vec2 screenToWorld(const glm::vec2& screenPos, int windowWidth, int windowHeight) const;

private:
    bool viewMatrixNeedsUpdate;
    bool projectionMatrixNeedsUpdate;
    bool aabbNeedsUpdate;

    glm::vec2 position;
    float zoom;
    float aspectRatio;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    AABB aabb;
};