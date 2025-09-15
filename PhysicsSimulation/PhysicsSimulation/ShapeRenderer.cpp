#include "ShapeRenderer.h"

std::unique_ptr<ShapeRenderer::ShapeRendererData> ShapeRenderer::srData;

void ShapeRenderer::initializeCircle()
{
	// Buffer objects
	float circleVertices[3 * 2] =
	{
		0.0f, 2.0f,
		1.7321f, -1.0f,
		-1.7321f, -1.0f
	};

	srData->circleVAO.bind();

	srData->circleVBO.bind();
	srData->circleVBO.setData(circleVertices, sizeof(circleVertices));

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	srData->circleVAO.unbind();

	// Shader
	std::vector<Shader::ShaderSource> circleShaderSources =
	{
		{GL_VERTEX_SHADER, "Shaders/circle.vert"},
		{GL_FRAGMENT_SHADER, "Shaders/circle.frag"}
	};
	srData->circleShader = std::make_shared<Shader>(circleShaderSources);
	GraphicsManager::addShader(srData->circleShader);
}

void ShapeRenderer::initializePolygon()
{
	// Buffer objects
	srData->polygonVAO.bind();

	srData->polygonVBO.bind();
	srData->polygonVBO.setData(nullptr, polygonShapeMaxVertices * sizeof(float) * 2, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	srData->polygonVAO.unbind();

	// Shader
	std::vector<Shader::ShaderSource> polygonShaderSources =
	{
		{GL_VERTEX_SHADER, "Shaders/polygon.vert"},
		{GL_FRAGMENT_SHADER, "Shaders/polygon.frag"}
	};
	srData->polygonShader = std::make_shared<Shader>(polygonShaderSources);
	GraphicsManager::addShader(srData->polygonShader);
}

void ShapeRenderer::initialize()
{
	srData = std::make_unique<ShapeRenderer::ShapeRendererData>();

	initializeCircle();
	initializePolygon();
}

void ShapeRenderer::shutdown()
{
	srData.release();
}

void ShapeRenderer::drawCircle(const glm::vec2& position, float radius, const glm::vec3& color)
{
	srData->circleShader->use();
	srData->circleVAO.bind();

	srData->circleShader->setVec2("position", position.x, position.y);
	srData->circleShader->setFloat("radius", radius);
	srData->circleShader->setVec3("color", color.x, color.y, color.z);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void ShapeRenderer::drawPolygon(const std::vector<glm::vec2>& vertices, const glm::vec3& color, bool isOutline)
{
	if (isOutline)
	{
		size_t verticesCount = vertices.size();
		if (verticesCount + 1 > polygonShapeMaxVertices)
		{
			return;
		}

		srData->polygonShader->use();
		srData->polygonVAO.bind();

		srData->polygonVBO.rewriteData(vertices.data(), verticesCount * sizeof(float) * 2);
		srData->polygonVBO.rewriteData(vertices.data(), sizeof(float) * 2, verticesCount * sizeof(float) * 2);

		srData->polygonShader->setVec3("color", color.x, color.y, color.z);

		glDrawArrays(GL_LINE_STRIP, 0, verticesCount + 1);
	}
	else
	{
		size_t verticesCount = vertices.size();
		if (verticesCount > polygonShapeMaxVertices)
		{
			return;
		}

		srData->polygonShader->use();
		srData->polygonVAO.bind();

		srData->polygonVBO.rewriteData(vertices.data(), verticesCount * sizeof(float) * 2);

		srData->polygonShader->setVec3("color", color.x, color.y, color.z);

		glDrawArrays(GL_TRIANGLE_FAN, 0, verticesCount);
	}
}

ShapeRenderer::ShapeRendererData::ShapeRendererData() :
	circleVAO(), circleVBO(), circleShader(),
	polygonVAO(), polygonVBO(), polygonShader()
{
}
