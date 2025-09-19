#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

#include "Shader.h"
#include "Camera.h"

class GraphicsManager
{
	struct GraphicsManagerData
	{
		GLFWwindow* window = nullptr;

		int windowWidth = 0;
		int windowHeight = 0;
		float aspectRatio = 0.0f;

		std::vector<std::shared_ptr<Shader>> shaders; // Should be only shaders used for rendering

		Camera camera;
	};

	static GraphicsManagerData gmData;
public:
	static void initialize(int windowWidth, int windowHeight);
	static void shutdown();

	// Window operations
	static void swapBuffersAndPollEvents();
	static GLFWwindow* getWindow();
	static bool failedToInitialize();
	static bool shouldClose();
	static void setTitle(const char* title);

	//
	static glm::vec2 screenToWorld(const glm::vec2& point);

	// Camera
	static Camera& getCamera();

	// Shader management
	static void addShader(const std::shared_ptr<Shader>& shader);
	static void sendMatricesToShaders();

	// Callbacks
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

