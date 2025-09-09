#include "GraphicsManager.h"
#include <iostream>

std::unique_ptr<GraphicsManager> GraphicsManager::instance;


void GraphicsManager::staticFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if (instance)
    {
        instance->framebufferSizeCallback(window, width, height);
    }
}


GraphicsManager::GraphicsManager(int windowWidth, int windowHeight) :
    windowWidth(windowWidth), windowHeight(windowHeight), aspectRatio((float)windowWidth / (float)windowHeight)
{
    if (instance)
    {
        std::cerr << "GraphicsManager instance already exists\n";
        return;
    }

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
    window = glfwCreateWindow(windowWidth, windowHeight, "---", nullptr, nullptr);
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
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, staticFramebufferSizeCallback);

    // Enable vsync
    glfwSwapInterval(1);
}

GraphicsManager::~GraphicsManager()
{
    if (window)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
	}
}


void GraphicsManager::initialize(int windowWidth, int windowHeight)
{
    if (instance)
    {
        std::cerr << "GraphicsManager instance already exists\n";
        return;
    }
    instance = std::make_unique<GraphicsManager>(windowWidth, windowHeight);
}

GraphicsManager* GraphicsManager::getInstance()
{
    return instance.get();
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

void GraphicsManager::setTitle(const char* title) const
{
    glfwSetWindowTitle(window, title);
}

glm::vec2 GraphicsManager::screenToWorld(const glm::vec2& point) const
{
    glm::vec2 worldPoint = { point.x / (float)windowWidth, point.y / (float)windowHeight };
    worldPoint = worldPoint * 2.0f - 1.0f;
    worldPoint.x *= aspectRatio;
    worldPoint.y *= -1.0f;
    return worldPoint;
}

void GraphicsManager::addShader(const std::shared_ptr<Shader>& shader)
{
	shaders.push_back(shader);
    shader->use();
	shader->setFloat("aspectRatio", aspectRatio);
}


void GraphicsManager::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
    for (const auto& shader : shaders)
    {
        shader->setFloat("aspectRatio", aspectRatio);
    }
}
