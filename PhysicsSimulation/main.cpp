#include "GraphicsManager.h"

#include <iostream>

#include "Shader.h"
#include "VAO.h"
#include "VBO.h"

#include "Simulation.h"
#include "Random.h"

void createCircleBuffers(VAO& vao, VBO& vbo, const float* vertices, size_t verticesSize)
{
    // Bind VAO
    vao.bind();

    // Bind and fill VBO
    vbo.bind();
    vbo.setData(vertices, verticesSize);

    // Configure vertex attributes
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO
    vao.unbind();
}

int main()
{
	// Initialize OpenGL and create window
	GraphicsManager::initialize(1200, 800);
    GraphicsManager& graphicsManager = *GraphicsManager::getInstance();
    if (graphicsManager.failedToInitialize())
    {
        return -1;
	}
    graphicsManager.setTitle("Physics simulation");

    // Shaders
    std::vector<Shader::ShaderSource> circleShaderSources =
    {
        {GL_VERTEX_SHADER, "Shaders/circle.vert"},
        {GL_FRAGMENT_SHADER, "Shaders/circle.frag"}
	};

	auto circleShader = std::make_shared<Shader>(circleShaderSources);
    graphicsManager.addShader(circleShader);

    // Buffer objects
    float circleVertices[3 * 2] =
    {
        0.0f, 2.0f,
		1.7321f, -1.0f,
		-1.7321f, -1.0f
    };

	VAO circleVAO;
	VBO circleVBO;
	createCircleBuffers(circleVAO, circleVBO, circleVertices, sizeof(circleVertices));

	// Simulation
	Simulation simulation;

    for (int i = 0; i < 10; i++)
    {
        float x = Random::Float(-0.5f, 0.5f);
        float y = Random::Float(-0.5f, 0.5f);

        float vx = Random::Float(-0.5f, 0.5f);
        float vy = Random::Float(-0.5f, 0.5f);

        float rot = 0.0f;
        float angVel = 0.0f;

        float radius = Random::Float(0.025f, 0.05f);
        
        float mass = 1.0f;
        float elasticity = 0.8f;

        simulation.addCircle({ x, y }, {vx, vy}, rot, angVel, radius, mass, elasticity);
	}

    //
    double previousTime = glfwGetTime();
    double uiUpdateTime = previousTime;
    int frameCount = 0;
    int updatesCount = 0;

    // Main loop
	while (!graphicsManager.shouldClose())
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

        // Update FPS every 0.5 seconds for stability
        if (currentTime - uiUpdateTime >= 0.5)
        {
            float fps = frameCount / (currentTime - uiUpdateTime);
            float ups = updatesCount / (currentTime - uiUpdateTime);
            float energy = simulation.calculateEnergy();

            uiUpdateTime = currentTime;
            frameCount = 0;
            updatesCount = 0;

            char title[64];
            snprintf(title, sizeof(title), "Physics simulation - FPS: %.1f - UPS: %.1f - E: %.1f", fps, ups, energy);
            graphicsManager.setTitle(title);
        }
        frameCount++;

        // Simulation
		updatesCount += simulation.update(deltaTime);

        // Rendering: set background color
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		// Draw circles
		circleShader->use();
        circleVAO.bind();

		for (const auto& body : simulation.getBodies())
        {
            if (body->shapeType != ShapeType::Circle)
            {
                continue;
            }

            RigidCircle* circle = dynamic_cast<RigidCircle*>(body.get());

            circleShader->setVec2("position", circle->position.x, circle->position.y);
            circleShader->setFloat("radius", circle->radius);

            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        // Swap buffers and poll events
		graphicsManager.swapBuffersAndPollEvents();
    }
    return 0;
}

// TODO: Add move/copy semantics
// TODO: Store same shape bodies on same vector. They should have move semantics for avoiding copies if new part is added or removed.
// TODO: Have separate vector that stores memory-safe pointers to all shapes.