#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

class GraphicsManager
{
	static std::unique_ptr<GraphicsManager> instance;

	//
	GLFWwindow* window = nullptr;

	int windowWidth = 0;
	int windowHeight = 0;

	static void staticFramebufferSizeCallback(GLFWwindow* window, int width, int height);
public:
	explicit GraphicsManager(int windowWidth, int windowHeight);
	~GraphicsManager();

	static void initialize(int windowWidth, int windowHeight);
	static GraphicsManager* getInstance();

	void swapBuffersAndPollEvents() const;

	GLFWwindow* getWindow() const;

	bool failedToInitialize() const;
	bool shouldClose() const;

	void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

