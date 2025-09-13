#include <iostream>

#include "GraphicsManager.h"
#include "Simulation.h"
#include "Random.h"
#include "InputManager.h"
#include "ShapeRenderer.h"

int main()
{
    // Initialize OpenGL and create window
    GraphicsManager::initialize(1200, 800);
    if (GraphicsManager::failedToInitialize())
    {
        return -1;
    }
    GraphicsManager::setTitle("Physics simulation");

    // Initialize InputManager
    InputManager::initialize(GraphicsManager::getWindow());

    // Initialize ShapeRenderer
    ShapeRenderer::initialize();

    // Simulation
    Simulation simulation;

    // Level boxes
    {
        float width = 1.4f;
        float height = 0.9f;
        float thickness = 0.2f;

        float mass = 0.0f;
        float inertia = 0.0f;

        Material material(1.0f, 0.0f, 0.0f);

        simulation.addBox({ 0.0f , -(height + thickness * 0.5f) }, { 0.0f, 0.0f }, 0.0f, 0.0f, mass, inertia, material, { width * 2.0f, thickness });
        simulation.addBox({ -(width + thickness * 0.5f), 0.0f }, { 0.0f, 0.0f }, 0.0f, 0.0f, mass, inertia, material, { thickness, height * 2.0f });
        simulation.addBox({ (width + thickness * 0.5f), 0.0f }, { 0.0f, 0.0f }, 0.0f, 0.0f, mass, inertia, material, { thickness, height * 2.0f });

        simulation.addBox({ 0.0f , 0.0f }, { 0.0f, 0.0f }, 0.25f, 0.0f, mass, inertia, material, { 2.0f, 0.05f });
    }

    //
    double previousTime = glfwGetTime();
    double uiUpdateTime = previousTime;
    int frameCount = 0;
    int updatesCount = 0;

    // Camera
    Camera& camera = GraphicsManager::getCamera();
    camera.setPosition({ 0.0f, 0.0f });
    camera.setZoom(1.0f);
    
    // Main loop
	while (!GraphicsManager::shouldClose())
    {
		// Time calculation
		double currentTime = glfwGetTime();
		float deltaTime = static_cast<float>(currentTime - previousTime);
        if (deltaTime > 0.5f)
        {
            deltaTime = 0.0f;
            uiUpdateTime = currentTime;
            frameCount = 0;
            updatesCount = 0;
        }
		previousTime = currentTime;

        // Input processing
        for (const auto& click : InputManager::getMouseClicks())
        {
            if (click.isLeftButton() && click.isPressed())
            {
                glm::vec2 position = GraphicsManager::screenToWorld({ click.xpos, click.ypos });

                float vx = 0.0f;
                float vy = 0.0f;

                float rot = 0.0f;
                float angVel = 0.0f;

                Material material(0.8f, 0.6f, 0.4f);

                float w = 0.1f;
                float h = 0.1f;

                float density = 600.0f;

                float mass = w * h * density;
                float inertia = mass * (w * w + h * h) / 12.0f;

                simulation.addBox(position, { vx, vy }, rot, angVel, mass, inertia, material, { w, h });
            }
            if (click.isRightButton() && click.isPressed())
            {
                glm::vec2 position = GraphicsManager::screenToWorld({ click.xpos, click.ypos });

                float vx = 0.0f;
                float vy = 0.0f;

                float rot = 0.0f;
                float angVel = 0.0f;

                Material material(0.8f, 0.6f, 0.4f);

                float density = 600.0f;

                float radius = 0.05f;

                float mass = 3.14f * radius * radius * density;
                float inertia = mass * radius * radius * 0.5f;

                for (int i = 0; i < 10; i++)
                {
                    glm::vec2 dpos = { Random::Float(-0.01f, 0.01f), Random::Float(-0.01f, 0.01f) };
                    simulation.addCircle(position + dpos, { vx, vy }, rot, angVel, mass, inertia, material, radius);
                }
            }
            if (click.isMiddleButton() && click.isPressed())
            {
                glm::vec2 position = GraphicsManager::screenToWorld({ click.xpos, click.ypos });

                float vx = 0.0f;
                float vy = 0.0f;

                float rot = 0.0f;
                float angVel = 0.0f;

                Material material(0.8f, 0.6f, 0.4f);

                float density = 600.0f;

                int verticesCount = Random::Int(4, 10);
                std::vector<glm::vec2> vertices;
                vertices.reserve(verticesCount);

                // Generate random angles
                std::vector<float> angles;
                angles.reserve(verticesCount);

                for (int i = 0; i < verticesCount; i++)
                {
                    float angle = Random::Float(0.0f, glm::two_pi<float>()); // random angle in radians
                    angles.push_back(angle);
                }

                // Sort angles to ensure consistent ordering around the circle
                std::sort(angles.begin(), angles.end());

                // Generate vertices in sorted order to ensure convexity
                for (float angle : angles)
                {
                    float cos_ = cosf(angle);
                    float sin_ = sinf(angle);
                    float distance = Random::Float(0.05f, 0.20f);
                    glm::vec2 vertex = glm::vec2(cos_, sin_) * distance;
                    vertices.push_back(vertex);
                }

                //
                auto polygonArea = [](const std::vector<glm::vec2>& vertices) -> float
                {
                    float area = 0.0f;
                    int n = static_cast<int>(vertices.size());

                    for (int i = 0; i < n; i++)
                    {
                        const glm::vec2& p1 = vertices[i];
                        const glm::vec2& p2 = vertices[(i + 1) % n]; // wrap to first
                        area += p1.x * p2.y - p2.x * p1.y;
                    }

                    return 0.5f * fabsf(area);
                };

                float area = polygonArea(vertices);
                float mass = area * density;
                float inertia = 1.0f;

                simulation.addPolygon(position, { vx, vy }, rot, angVel, mass, inertia, material, vertices);
            }
        }
        InputManager::clearInputs();

        {
            // Move
            bool resetCamera = InputManager::isKeyPressed(GLFW_KEY_R);

            if (resetCamera)
            {
                camera.setPosition({ 0.0f, 0.0f });
                camera.setZoom(1.0f);
            }
            else
            {
                bool right = InputManager::isKeyPressed(GLFW_KEY_D);
                bool left = InputManager::isKeyPressed(GLFW_KEY_A);

                bool up = InputManager::isKeyPressed(GLFW_KEY_W);
                bool down = InputManager::isKeyPressed(GLFW_KEY_S);

                glm::vec2 moveVector =
                {
                    (float)right - (float)left,
                    (float)up - (float)down
                };

                camera.move(moveVector * deltaTime * 2.0f / camera.getZoom());
            }

            // Zoom
            bool zoomIn = InputManager::isKeyPressed(GLFW_KEY_Q);
            bool zoomOut = InputManager::isKeyPressed(GLFW_KEY_E);

            float zoomFactor = 1.0f;
            const float zoomSpeed = 0.1f; // smaller = smoother

            if (zoomIn)
            {
                zoomFactor *= 1.0f + zoomSpeed;
            }
            if (zoomOut)
            {
                zoomFactor /= 1.0f + zoomSpeed;
            }
            if (zoomFactor != 1.0f)
            {
                float zoom = camera.getZoom();
                camera.zoomBy(zoomFactor);
            }
        }

        // Simulation
		updatesCount += simulation.update(deltaTime);

        // Rendering: set background color
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //
        GraphicsManager::sendMatricesToShaders();

		// Draw circles
		for (const auto& body : simulation.getBodies())
        {
            if (body->shapeType != ShapeType::Circle)
            {
                continue;
            }

            RigidCircle* circle = dynamic_cast<RigidCircle*>(body.get());

            ShapeRenderer::drawCircle(circle->position, circle->radius, { 1.0f, 1.0f, 1.0f });

            float cos_ = cosf(body->rotation);
            float sin_ = sinf(body->rotation);
            std::vector<glm::vec2> vertices
            {
                circle->position, circle->position + glm::vec2(cos_, sin_) * circle->radius
            };

            ShapeRenderer::drawPolygon(vertices, { 0.0f, 0.0f, 0.0f }, true);
        }

        // Draw polygons
        for (const auto& body : simulation.getBodies())
        {
            if (body->shapeType != ShapeType::Polygon)
            {
                continue;
            }

            RigidPolygon* polygon = dynamic_cast<RigidPolygon*>(body.get());
            const auto& vertices = polygon->getTransformedVertices();

            ShapeRenderer::drawPolygon(vertices, { 1.0f, 1.0f, 1.0f });
        }

        // Draw Quadtree
        {
            std::vector<AABB> bounds;
            simulation.getQuadtreeBounds(bounds);

            for (const auto& aabb : bounds)
            {
                std::vector<glm::vec2> vertices =
                {
                    aabb.min, {aabb.min.x, aabb.max.y}, aabb.max, {aabb.max.x, aabb.min.y}
                };

                size_t verticesCount = vertices.size();
                ShapeRenderer::drawPolygon(vertices, { 1.0f, 0.0f, 0.0f }, true);
            }
        }

        /*for (const auto& body : simulation.getBodies())
        {
            if (body->isStatic())
            {
                continue;
            }

            const AABB& aabb = body->getAABB();

            std::vector<glm::vec2> vertices =
            {
                aabb.min, {aabb.min.x, aabb.max.y}, aabb.max, {aabb.max.x, aabb.min.y}
            };

            size_t verticesCount = vertices.size();
            ShapeRenderer::drawPolygon(vertices, { 1.0f, 0.0f, 0.0f }, true);
        }*/

        // Draw contacts
        /*for (const auto& manifold : Collisions::getManifolds())
        {
            ShapeRenderer::drawCircle(manifold.contacts[0], 0.01f, { 0.0f, 1.0f, 0.0f });
            if (manifold.countOfContacts == 2)
            {
                ShapeRenderer::drawCircle(manifold.contacts[1], 0.01f, { 0.0f, 1.0f, 0.0f });
            }
        }*/

        // Update window title
        if (currentTime - uiUpdateTime >= 0.5)
        {
            float fps = frameCount / (currentTime - uiUpdateTime);
            float ups = updatesCount / (currentTime - uiUpdateTime);
            size_t bodiesCount = simulation.getBodies().size();

            uiUpdateTime = currentTime;
            frameCount = 0;
            updatesCount = 0;

            char title[64];
            snprintf(title, sizeof(title), "Physics simulation - FPS: %.1f - UPS: %.1f - BODIES: %i", fps, ups, (int)bodiesCount);
            GraphicsManager::setTitle(title);
        }
        frameCount++;

        // Swap buffers and poll events
        GraphicsManager::swapBuffersAndPollEvents();
    }
    ShapeRenderer::shutdown();
    GraphicsManager::shutdown();
    return 0;
}

// TODO: Store same shape bodies on same vector. They should have move semantics for avoiding copies if new part is added or removed.
// TODO: Have separate vector that stores memory-safe pointers to all shapes.
// 
// TODO: Have optimization for boxes. You have to do less calculations for SAT.
// TODO: Check if object's AABB crosses screen's AABB to determine, draw or not?
// TODO: Batch shapes of same type to reduce drawcalls. Follow order!
// TODO: Don't use aspectRatio uniform. Use matrix instead.
// TODO: Avoid rebinding shaders with each draw method call.
// TODO: Add calculateInertia method to each shape type
// TODO: Store a pointer to a material instead of storing material
// TODO: CollisionManifold: Try storing raw pointers.
// TODO: Objects stacked atop of each other tend up to push objects above them away.
//
// TODO: Space partitioning. Add object pool for QuadtreeNode pointers.
// TODO: Calculate body's mass center for correctly applying forces.
// TODO: Simulation supports only convex polygon. Add support for convave ones.
// TODO: Very jittery when many objects. Maybe because objects collision manifolds generate once, but bodies can to not collide after.
// TODO: QuadtreeNode's objects vector can get very big and then get into the pool with big capacity. Then it can be used as a top node, which means it takes more memory unnecessary.