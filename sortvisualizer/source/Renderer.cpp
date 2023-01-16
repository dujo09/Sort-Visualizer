#include "Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Renderer::Renderer(const char* vertexShaderPath, const char* fragmentShaderPath, 
	const float screenWidth, const float screenHeight):
	m_defaultShader(vertexShaderPath, fragmentShaderPath), 
	m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{
	float quadVertices[] = {
		0.0f,	-1.0f,
		1.0f,	 0.0f,
		0.0f,	 0.0f,

		0.0f,	-1.0f,
		1.0f,	-1.0f,
		1.0f,	 0.0f,
	};

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// set projection matrix once (window is not resizeable)
	glm::mat4 projectionMatrix = glm::ortho(0.0f, screenWidth, screenHeight, 0.0f, -1.0f, 1.0f);
	m_defaultShader.use();
	m_defaultShader.setMat4("projectionMatrix", projectionMatrix);
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void Renderer::renderVectorAsRectangles(const std::vector<Sortable>& items)
{
	const float rectangleWidth = m_screenWidth / items.size();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_defaultShader.use();
	glBindVertexArray(m_VAO);

	for (int i = 0; i < items.size(); ++i)
	{
		const float rectangleHeight = items[i].getValue() / items.size() * m_screenHeight;

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(rectangleWidth * i, m_screenHeight, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(rectangleWidth, rectangleHeight, 1.0f));

		m_defaultShader.setMat4("modelMatrix", modelMatrix);
		m_defaultShader.setVec3("color", items[i].getColor());

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}
