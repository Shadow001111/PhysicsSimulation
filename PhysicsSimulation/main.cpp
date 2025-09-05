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
	GraphicsManager graphicsManager;
    if (graphicsManager.failedToInitialize())
    {
        return -1;
	}

    // Shaders
    std::vector<Shader::ShaderSource> circleShaderSources =
    {
        {GL_VERTEX_SHADER, "Shaders/circle.vert"},
        {GL_FRAGMENT_SHADER, "Shaders/circle.frag"}
	};
	Shader circleShader(circleShaderSources);

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
        float x = Random::Float(-1.0f, 1.0f);
        float y = Random::Float(-1.0f, 1.0f);
        float radius = Random::Float(0.1f, 0.5f);
        simulation.addCircle({ x, y }, radius);
	}

    // Main loop
	while (!graphicsManager.shouldClose())
    {
        // Rendering: set background color
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		// Draw circles
		circleShader.use();
        circleVAO.bind();

		for (const RigidCircle& circle : simulation.getCircles())
        {
            circleShader.setVec2("position", circle.position.x, circle.position.y);
            circleShader.setFloat("radius", circle.radius);

            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        // Swap buffers and poll events
		graphicsManager.swapBuffersAndPollEvents();
    }
    return 0;
}
