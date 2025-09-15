#include "Camera.h"

void Camera::updateViewMatrix()
{
    // Create view matrix: translate by negative position, then scale by zoom
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::scale(viewMatrix, glm::vec3(zoom, zoom, 1.0f));
    viewMatrix = glm::translate(viewMatrix, glm::vec3(-position, 0.0f));
}

void Camera::updateProjectionMatrix()
{
    float halfHeight = 1.0f;
    float halfWidth = halfHeight * aspectRatio;
    projectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
}

void Camera::updateAABB()
{
    float halfHeight = 1.0f / zoom;
    float halfWidth = halfHeight * aspectRatio;

    aabb.min = { position.x - halfWidth, position.y - halfHeight };
    aabb.max = { position.x + halfWidth, position.y + halfHeight };
}

Camera::Camera(const glm::vec2& position, float zoom)
    : viewMatrixNeedsUpdate(true), projectionMatrixNeedsUpdate(true), aabbNeedsUpdate(true),
    position(position), zoom(zoom), aspectRatio(1.0f), viewMatrix(1.0f), projectionMatrix(1.0f)
{
    
}

const glm::vec2& Camera::getPosition() const
{
    return position;
}

float Camera::getZoom() const
{
    return zoom;
}

const glm::mat4& Camera::getViewMatrix()
{
    if (viewMatrixNeedsUpdate)
    {
        updateViewMatrix();
        viewMatrixNeedsUpdate = false;
    }
    return viewMatrix;
}

const glm::mat4& Camera::getProjectionMatrix()
{
    if (projectionMatrixNeedsUpdate)
    {
        updateProjectionMatrix();
        projectionMatrixNeedsUpdate = false;
    }
    return projectionMatrix;
}

const AABB& Camera::getAABB()
{
    if (aabbNeedsUpdate)
    {
        updateAABB();
        aabbNeedsUpdate = false;
    }
    return aabb;
}

void Camera::setPosition(const glm::vec2& newPosition)
{
    position = newPosition;
    viewMatrixNeedsUpdate = true;
    aabbNeedsUpdate = true;
}

void Camera::setZoom(float newZoom)
{
    zoom = newZoom;
    viewMatrixNeedsUpdate = true;
    aabbNeedsUpdate = true;
}

void Camera::move(const glm::vec2& offset)
{
    position += offset;
    viewMatrixNeedsUpdate = true;
    aabbNeedsUpdate = true;
}

void Camera::zoomBy(float factor)
{
    setZoom(zoom * factor);
}

void Camera::setAspectRatio(float aspectRatio)
{
    this->aspectRatio = aspectRatio;
    projectionMatrixNeedsUpdate = true;
    aabbNeedsUpdate = true;
}

glm::vec2 Camera::screenToWorld(const glm::vec2& screenPos, int windowWidth, int windowHeight) const
{
    // Convert screen coordinates to normalized device coordinates
    glm::vec2 ndc =
    {
        (screenPos.x / (float)windowWidth) * 2.0f - 1.0f,
        -((screenPos.y / (float)windowHeight) * 2.0f - 1.0f) // Flip Y axis
    };

    // Apply aspect ratio
    ndc.x *= aspectRatio;

    // Convert from NDC to world space
    glm::vec2 worldPos = ndc / zoom + position;

    return worldPos;
}