#include "SortController.h"
#include "Shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>


void processInput(GLFWwindow* window, SortController* sortContoller);

const float SCREEN_WIDTH = 1600.0f;
const float SCREEN_HEIGHT = 800.0f;

// sort settings
const unsigned int NUMBER_OF_ITEMS = 50;
const unsigned int TIME_STEP_MICROSECONDS = 500;
const SortType SORT_TYPE = BUBBLE_SORT;

int main() {
	// glfw: initialise and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sort Visualizer", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load GLAD function\n";
		glfwTerminate();
		return -1;
	}

	// setup vertices and buffers
	float quadVertices[] = {
		0.0f,	-1.0f,
		1.0f,	 0.0f,
		0.0f,	 0.0f,

		0.0f,	-1.0f,
		1.0f,	-1.0f,
		1.0f,	 0.0f,
	};
	
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// create sort controller
	SortController sortController = SortController(NUMBER_OF_ITEMS, TIME_STEP_MICROSECONDS, SORT_TYPE);
	// create default shader
	Shader defaultShader = Shader("shaders/DefaultShader.vert", "shaders/DefaultShader.frag");

	// set projection matrix once (window is not resizeable)
	glm::mat4 projectionMatrix = glm::ortho(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);
	defaultShader.use();
	defaultShader.setMat4("projectionMatrix", projectionMatrix);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window, &sortController);

		float rectangleWidth = SCREEN_WIDTH / sortController.getNumberOfItems();
	
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		defaultShader.use();
		glBindVertexArray(quadVAO);

		for (int i = 0; i < sortController.getNumberOfItems(); ++i)
		{
			// setup model matrix for each item
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix,
				glm::vec3(i * rectangleWidth, SCREEN_HEIGHT, 0.0f));
			modelMatrix = glm::scale(modelMatrix,
				glm::vec3(rectangleWidth, sortController.getItems()[i].getValue() / sortController.getNumberOfItems() * SCREEN_HEIGHT, 1.0f));

			// set model matrix uniform
			defaultShader.setMat4("modelMatrix", modelMatrix);

			// set color uniform for each item
			defaultShader.setVec3("color", sortController.getItems()[i].getColor());

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow* window, SortController* sortController)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		sortController->setTimeStep(TIME_STEP_MICROSECONDS / 100);
	}
	else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
	{
		sortController->setTimeStep(TIME_STEP_MICROSECONDS);
	}

	if (!sortController->isSorting())
	{
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			sortController->shuffleItems();
		}
		else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		{
			sortController->startSort();
		}
	}
}
