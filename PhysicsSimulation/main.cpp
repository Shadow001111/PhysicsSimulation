#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"
#include "VAO.h"
#include "VBO.h"

int initializeOpenGL(GLFWwindow*& window)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Configure GLFW for OpenGL 4.6 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    window = glfwCreateWindow(800, 600, "Physics simulation", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load OpenGL functions with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

    // Set viewport
    glViewport(0, 0, 800, 600);
	return 0;
}

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
    GLFWwindow* window;
    if (initializeOpenGL(window) != 0)
    {
        return -1;
    }

    // Shader
    std::vector<Shader::ShaderSource> circleShaderSources =
    {
        {GL_VERTEX_SHADER, "Shaders/circle.vert"},
        {GL_FRAGMENT_SHADER, "Shaders/circle.frag"}
	};
	Shader shader(circleShaderSources);

    // Circle buffers
    float circleVertices[3 * 2] =
    {
        0.0f, 2.0f,
		1.7321f, -1.0f,
		-1.7321f, -1.0f
    };

	VAO circleVAO;
	VBO circleVBO;
	createCircleBuffers(circleVAO, circleVBO, circleVertices, sizeof(circleVertices));

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Rendering: set background color
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		// Draw circle
		shader.use();
		shader.setVec2("position", 0.0f, 0.0f);
		shader.setFloat("radius", 1.0f);

		circleVAO.bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
