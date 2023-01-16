#pragma once

#include "Shader.h"
#include "Sortable.h"

#include <vector>

class Renderer
{
private:
	unsigned int m_VAO = 0, m_VBO = 0;
	Shader m_defaultShader;
	const float m_screenWidth, m_screenHeight;
public:
	Renderer(const char* vertexShaderPath, const char* fragmentShaderPath,
		const float screenWidth, const float screenHeight);
	~Renderer();
	
	// render methods
	void renderVectorAsRectangles(std::vector<Sortable> items);
};

