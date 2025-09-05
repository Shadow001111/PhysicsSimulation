#include "GraphicsManager.h"
#include <iostream>

GraphicsManager::GraphicsManager()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return;
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
        window = nullptr;
        return;
    }
    glfwMakeContextCurrent(window);

    // Load OpenGL functions with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
		window = nullptr;
        return;
    }

    // Set viewport
    glViewport(0, 0, 800, 600);
}

GraphicsManager::~GraphicsManager()
{
    if (window)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
	}
}

void GraphicsManager::swapBuffersAndPollEvents() const
{
    if (window)
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
	}
}

GLFWwindow* GraphicsManager::getWindow() const
{
    return window;
}

bool GraphicsManager::failedToInitialize() const
{
	return window == nullptr;
}

bool GraphicsManager::shouldClose() const
{
	return glfwWindowShouldClose(window);
}
