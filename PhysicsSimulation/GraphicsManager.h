#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

#include "Shader.h"

class GraphicsManager
{
	static std::unique_ptr<GraphicsManager> instance;

	//
	GLFWwindow* window = nullptr;

	int windowWidth = 0;
	int windowHeight = 0;
	float aspectRatio = 0.0f;

	std::vector<std::shared_ptr<Shader>> shaders; // Should be only shaders used for rendering

	static void staticFramebufferSizeCallback(GLFWwindow* window, int width, int height);
public:
	explicit GraphicsManager(int windowWidth, int windowHeight);
	~GraphicsManager();

	// SIngleton
	static void initialize(int windowWidth, int windowHeight);
	static GraphicsManager* getInstance();

	// Window operations
	void swapBuffersAndPollEvents() const;
	GLFWwindow* getWindow() const;
	bool failedToInitialize() const;
	bool shouldClose() const;
	void setTitle(const char* title) const;

	// Shader management
	void addShader(const std::shared_ptr<Shader>& shader);

	// Callbacks
	void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

