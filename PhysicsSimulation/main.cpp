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

    // Level boxex
    {
        float width = 2.0f;
        float height = 0.9f;
        float thickness = 0.2f;

        simulation.addBox({ 0.0f , -(height + thickness * 0.5f) }, { 0.0f, 0.0f }, 0.0f, 0.0f, 0.0f, 1.0f, { width * 2.0f, thickness });
        simulation.addBox({ -(width + thickness * 0.5f), 0.0f }, { 0.0f, 0.0f }, 0.0f, 0.0f, 0.0f, 1.0f, { thickness, width * 2.0f });
        simulation.addBox({ (width + thickness * 0.5f), 0.0f }, { 0.0f, 0.0f }, 0.0f, 0.0f, 0.0f, 1.0f, { thickness, width * 2.0f });

        simulation.addBox({ 0.0f , 0.0f }, { 0.0f, 0.0f }, 0.0f, 0.25f, 0.0f, 1.0f, { 3.0f, 0.05f });
    }

    // Circles
    for (int i = 0; i < 0; i++)
    {
        float x = Random::Float(-0.5f, 0.5f);
        float y = Random::Float(-0.5f, 0.5f);

        float vx = Random::Float(-0.5f, 0.5f);
        float vy = Random::Float(-0.5f, 0.5f);

        float rot = 0.0f;
        float angVel = 0.0f;

        float mass = 1.0f;
        float elasticity = 0.9f;

        float radius = Random::Float(0.025f, 0.05f);

        simulation.addCircle({ x, y }, {vx, vy}, rot, angVel, mass, elasticity, radius);
	}

    // Boxes
    for (int i = 0; i < 0; i++)
    {
        float x = Random::Float(-0.5f, 0.5f);
        float y = Random::Float(-0.5f, 0.5f);

        float vx = Random::Float(-0.5f, 0.5f);
        float vy = Random::Float(-0.5f, 0.5f);

        float rot = 0.0f;
        float angVel = 0.0f;

        float mass = 1.0f;
        float elasticity = 0.9f;

        float w = Random::Float(0.025f, 0.05f) * 2.0f;
        float h = Random::Float(0.025f, 0.05f) * 2.0f;

        simulation.addBox({ x, y }, { vx, vy }, rot, angVel, mass, elasticity, { w, h });
    }

    //
    double previousTime = glfwGetTime();
    double uiUpdateTime = previousTime;
    int frameCount = 0;
    int updatesCount = 0;
    
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

                float mass = 1.0f;
                float elasticity = 0.8f;

                float w = 0.1f;
                float h = 0.1f;

                simulation.addBox(position, { vx, vy }, rot, angVel, mass, elasticity, { w, h });
            }
            if (click.isRightButton() && click.isPressed())
            {
                glm::vec2 position = GraphicsManager::screenToWorld({ click.xpos, click.ypos });

                float vx = 0.0f;
                float vy = 0.0f;

                float rot = 0.0f;
                float angVel = 0.0f;

                float mass = 1.0f;
                float elasticity = 0.8f;

                float radius = 0.05f;

                simulation.addCircle(position, { vx, vy }, rot, angVel, mass, elasticity, radius);
            }
        }
        InputManager::clearInputs();

        // Simulation
		updatesCount += simulation.update(deltaTime);

        // Rendering: set background color
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		// Draw circles
		for (const auto& body : simulation.getBodies())
        {
            if (body->shapeType != ShapeType::Circle)
            {
                continue;
            }

            RigidCircle* circle = dynamic_cast<RigidCircle*>(body.get());

            ShapeRenderer::drawCircle(circle->position, circle->radius, { 1.0f, 1.0f, 1.0f });
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

        // Draw AABBs
        for (const auto& body : simulation.getBodies())
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
        }

        // Update FPS every 0.5 seconds for stability
        if (currentTime - uiUpdateTime >= 0.5)
        {
            float fps = frameCount / (currentTime - uiUpdateTime);
            float ups = updatesCount / (currentTime - uiUpdateTime);

            uiUpdateTime = currentTime;
            frameCount = 0;
            updatesCount = 0;

            char title[64];
            snprintf(title, sizeof(title), "Physics simulation - FPS: %.1f - UPS: %.1f", fps, ups);
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

// TODO: Add move/copy semantics
// TODO: Store same shape bodies on same vector. They should have move semantics for avoiding copies if new part is added or removed.
// TODO: Have separate vector that stores memory-safe pointers to all shapes.
// TODO: Have optimization for boxes. You have to do less calculations for SAT.
// TODO: Check if object's AABB crosses screen's AABB to determine, draw or not?
// TODO: Batch shapes of same type to reduce drawcalls. Follow order!
// TODO: Don't use aspectRatio uniform. Use matrix instead.
// TODO: Avoid rebinding shaders with each draw method call.