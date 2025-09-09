#include "GraphicsManager.h"
#include <iostream>

GraphicsManager::GraphicsManagerData GraphicsManager::gmData;


void GraphicsManager::initialize(int windowWidth, int windowHeight)
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
    gmData.windowWidth = windowWidth;
    gmData.windowHeight = windowHeight;
    gmData.aspectRatio = (float)windowWidth / (float)windowHeight;

    gmData.window = glfwCreateWindow(windowWidth, windowHeight, "---", nullptr, nullptr);
    if (!gmData.window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        gmData.window = nullptr;
        return;
    }
    glfwMakeContextCurrent(gmData.window);

    // Load OpenGL functions with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        gmData.window = nullptr;
        return;
    }

    // Set viewport
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(gmData.window, framebufferSizeCallback);

    // Enable vsync
    glfwSwapInterval(1);
}

void GraphicsManager::shutdown()
{
    if (gmData.window)
    {
        glfwDestroyWindow(gmData.window);
        glfwTerminate();
        gmData.window = nullptr;
        gmData.shaders.clear();
    }
}


void GraphicsManager::swapBuffersAndPollEvents()
{
    if (gmData.window)
    {
        glfwSwapBuffers(gmData.window);
        glfwPollEvents();
	}
}


GLFWwindow* GraphicsManager::getWindow()
{
    return gmData.window;
}


bool GraphicsManager::failedToInitialize()
{
	return gmData.window == nullptr;
}

bool GraphicsManager::shouldClose()
{
	return glfwWindowShouldClose(gmData.window);
}

void GraphicsManager::setTitle(const char* title)
{
    glfwSetWindowTitle(gmData.window, title);
}

glm::vec2 GraphicsManager::screenToWorld(const glm::vec2& point)
{
    glm::vec2 worldPoint = { point.x / (float)gmData.windowWidth, point.y / (float)gmData.windowHeight };
    worldPoint = worldPoint * 2.0f - 1.0f;
    worldPoint.x *= gmData.aspectRatio;
    worldPoint.y *= -1.0f;
    return worldPoint;
}

void GraphicsManager::addShader(const std::shared_ptr<Shader>& shader)
{
    gmData.shaders.push_back(shader);
    shader->use();
	shader->setFloat("aspectRatio", gmData.aspectRatio);
}


void GraphicsManager::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    gmData.windowWidth = width;
    gmData.windowHeight = height;
    gmData.aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
    for (const auto& shader : gmData.shaders)
    {
        shader->setFloat("aspectRatio", gmData.aspectRatio);
    }
}
