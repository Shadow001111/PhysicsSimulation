#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GraphicsManager
{
	GLFWwindow* window = nullptr;
public:
	GraphicsManager();
	~GraphicsManager();

	void swapBuffersAndPollEvents() const;

	GLFWwindow* getWindow() const;

	bool failedToInitialize() const;
	bool shouldClose() const;
};

