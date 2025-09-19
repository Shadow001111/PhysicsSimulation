#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "GraphicsManager.h"
#include "Graphics/OpenGL/VAO.h"
#include "Graphics/OpenGL/VBO.h"

class ShapeRenderer
{
	static const size_t polygonShapeMaxVertices = 256;

	struct ShapeRendererData
	{
		VAO circleVAO;
		VBO circleVBO;
		std::shared_ptr<Shader> circleShader;

		VAO polygonVAO;
		VBO polygonVBO;
		std::shared_ptr<Shader> polygonShader;

		ShapeRendererData();
	};

	static std::unique_ptr<ShapeRendererData> srData;

	static void initializeCircle();
	static void initializePolygon();
public:
	static void initialize();
	static void shutdown();

	static void drawCircle(const glm::vec2& position, float radius, const glm::vec3& color);
	static void drawPolygon(const std::vector<glm::vec2>& vertices, const glm::vec3& color, bool isOutline = false);
};

