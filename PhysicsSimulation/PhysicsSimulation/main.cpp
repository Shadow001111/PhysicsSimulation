#include <iostream>
#include <windows.h>
#undef min, max

#include "Graphics/GraphicsManager.h"
#include "Graphics/ShapeRenderer.h"

#include "Physics/Simulation.h"

#include "Core/Random.h"
#include "Core/CoreMath.h"

#include "Input/InputManager.h"

static void drawBodies(const std::vector<std::unique_ptr<RigidBody>>& bodies)
{
    for (const auto& body : bodies)
    {
        if (body->shapeType == ShapeType::Circle)
        {
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
        else if (body->shapeType == ShapeType::Polygon)
        {
            RigidPolygon* polygon = dynamic_cast<RigidPolygon*>(body.get());
            const auto& vertices = polygon->getTransformedVertices();

            ShapeRenderer::drawPolygon(vertices, { 1.0f, 1.0f, 1.0f });
        }

        // Center of mass
        {
            const glm::vec2& centerOfMass = body->getCenterOfMass();
            ShapeRenderer::drawCircle(centerOfMass, 0.01f, { 1.0f, 0.0f, 0.0f });
        }
    }
}

static void drawSpatialStructures(Simulation& simulation)
{
    CollisionDetectionMethod method = simulation.getCollisionDetectionMethod();

    if (method == CollisionDetectionMethod::Quadtree)
    {
        std::vector<AABB> bounds;
        simulation.getQuadtreeBounds(bounds);

        for (const auto& aabb : bounds)
        {
            std::vector<glm::vec2> vertices =
            {
                aabb.min, {aabb.min.x, aabb.max.y}, aabb.max, {aabb.max.x, aabb.min.y}
            };

            ShapeRenderer::drawPolygon(vertices, { 1.0f, 0.0f, 0.0f }, true);
        }
    }
    else if (method == CollisionDetectionMethod::SpatialHashGrid)
    {
        std::vector<AABB> bounds;
        simulation.getHashGridBounds(bounds, true); // Only show active cells

        for (const auto& aabb : bounds)
        {
            std::vector<glm::vec2> vertices =
            {
                aabb.min, {aabb.min.x, aabb.max.y}, aabb.max, {aabb.max.x, aabb.min.y}
            };

            ShapeRenderer::drawPolygon(vertices, { 0.0f, 1.0f, 0.0f }, true);
        }
    }
}

int main()
{
    {
        HWND console = GetConsoleWindow();
        ShowWindow(console, SW_MINIMIZE);
    }
    
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

    // Materials
    std::unique_ptr<Material> materialLevel = std::make_unique<Material>(0.0f, 0.0f, 0.0f);
    std::unique_ptr<Material> materialBody = std::make_unique<Material>(0.8f, 0.6f, 0.4f);

    // Level boxes
    {
        float width = 2.0f;
        float height = 2.0f;
        float thickness = 0.2f;

        float mass = 0.0f;
        float inertia = 0.0f;

        simulation.addBox({ 0.0f , -(height + thickness * 0.5f) }, { 0.0f, 0.0f }, 0.0f, 0.0f, mass, inertia, materialLevel.get(), {width * 2.0f, thickness});
        simulation.addBox({ 0.0f , (height + thickness * 0.5f) }, { 0.0f, 0.0f }, 0.0f, 0.0f, mass, inertia, materialLevel.get(), { width * 2.0f, thickness });
        simulation.addBox({ -(width + thickness * 0.5f), 0.0f }, { 0.0f, 0.0f }, 0.0f, 0.0f, mass, inertia, materialLevel.get(), { thickness, height * 2.0f });
        simulation.addBox({ (width + thickness * 0.5f), 0.0f }, { 0.0f, 0.0f }, 0.0f, 0.0f, mass, inertia, materialLevel.get(), { thickness, height * 2.0f });
    }

    {
        
        float width = 4.0f;
        float height = 0.1f;

        float density = 600.0f;

        auto rotatingBox = simulation.addBox({ 0.0f, 0.0f }, { 0.0f, 0.0f }, 0.0f, 0.0f, 0.0f, 0.0f, materialBody.get(), { width, height }, density);
        simulation.addAxisConstraint(rotatingBox, true, true);
        simulation.addAngularVelocityConstraint(rotatingBox, 4.0f);
    }

    //
    double previousTime = glfwGetTime();
    double uiUpdateTime = previousTime;
    int frameCount = 0;
    int updatesCount = 0;

    double perfomancePrintTime = previousTime + 1.0f;

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

                float w = 0.1f;
                float h = 0.1f;

                float density = 600.0f;

                simulation.addBox(position, { vx, vy }, rot, angVel, 0.0f, 0.0f, materialBody.get(), { w, h }, density);
            }
            if (click.isRightButton() && click.isPressed())
            {
                glm::vec2 position = GraphicsManager::screenToWorld({ click.xpos, click.ypos });

                float vx = 0.0f;
                float vy = 0.0f;

                float rot = 0.0f;
                float angVel = 0.0f;

                float density = 600.0f;

                float radius = 0.05f;

                for (int i = 0; i < 100; i++)
                {
                    glm::vec2 dpos = { Random::Float(-1.0f, 1.0f), Random::Float(-1.0f, 1.0f) };
                    dpos *= radius * 3.0f;
                    simulation.addCircle(position + dpos, { vx, vy }, rot, angVel, 0.0f, 0.0f, materialBody.get(), radius, density);
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

                int verticesCount = Random::Int(3, 10);
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

                // Generate vertices
                for (float angle : angles)
                {
                    float distance = Random::Float(0.05f, 0.20f);
                    glm::vec2 vertex = CoreMath::rotatePoint( {1.0f, 0.0f}, angle ) * distance;
                    vertices.push_back(vertex);
                }

                //
                auto body = simulation.addPolygon(position, { vx, vy }, rot, angVel, 0.0f, 0.0f, materialBody.get(), vertices, density);
                //simulation.addAxisConstraint(body, true, true);
            }
        }

        for (const auto& key : InputManager::getKeyActions())
        {
            if (key.key == GLFW_KEY_R)
            {
                if (key.isPressed())
                {
                    // Reset camera
                    camera.setPosition({ 0.0f, 0.0f });
                    camera.setZoom(1.0f);
                }
            }
            else if (key.key == GLFW_KEY_T)
            {
                if (key.isPressed())
                {
                    int method = (int)simulation.getCollisionDetectionMethod();
                    int nextmethod = (method + 1) % (int)CollisionDetectionMethod::_COUNT;
                    simulation.setCollisionDetectionMethod((CollisionDetectionMethod)nextmethod);
                }
            }
        }

        InputManager::clearInputs();

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

            // Zoom
            bool zoomIn = InputManager::isKeyPressed(GLFW_KEY_Q);
            bool zoomOut = InputManager::isKeyPressed(GLFW_KEY_E);

            float zoomFactor = 1.0f;
            const float zoomSpeed = 5.0f;

            if (zoomIn)
            {
                zoomFactor *= 1.0f + zoomSpeed * deltaTime;
            }
            if (zoomOut)
            {
                zoomFactor /= 1.0f + zoomSpeed * deltaTime;
            }
            if (zoomFactor != 1.0f)
            {
                float zoom = camera.getZoom();
                camera.zoomBy(zoomFactor);
            }
        }

        // Simulation
		updatesCount += simulation.update(deltaTime);

        // Profiler
        if (currentTime > perfomancePrintTime)
        {
            perfomancePrintTime = currentTime + 1.0f;
            simulation.printPerfomanceReport();
        }

        // Rendering: set background color
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //
        GraphicsManager::sendMatricesToShaders();

        // Draw bodies
        {
            const auto& bodies = simulation.getBodies();
            drawBodies(bodies);
        }

        // Draw spatial data structures
        //drawSpatialStructures(simulation);

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
// TODO: Avoid rebinding shaders with each draw method call.
// TODO: Add calculateInertia method to each shape type
// TODO: Objects stacked atop of each other tend up to push objects above them away.
//
// TODO: Calculate body's mass center for correctly applying forces.
// TODO: Simulation supports only convex polygon. Add support for concave ones.
// TODO: Add sleeping
// TODO: Maybe combine friction using: sqrt(fric1 * fric2)