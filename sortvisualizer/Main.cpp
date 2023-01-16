#include "SortController.h"
#include "Shader.h"
#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>


void processInput(GLFWwindow* window, SortController* sortContoller);

const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGHT = 600.0f;

// sort settings (temporary)
const unsigned int NUMBER_OF_ITEMS = 40;
const unsigned int TIME_STEP_MICROSECONDS = 500000;
const SortType SORT_TYPE = BUBBLE_SORT;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sort Visualizer", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load GLAD function\n";
		glfwTerminate();
		return -1;
	}	

	SortController sortController = SortController(NUMBER_OF_ITEMS, TIME_STEP_MICROSECONDS, SORT_TYPE);
	Renderer sortRenderer = Renderer("shaders/DefaultShader.vert", "shaders/DefaultShader.frag", SCREEN_WIDTH, SCREEN_HEIGHT);
	
	while (!glfwWindowShouldClose(window))
	{
		processInput(window, &sortController);

		sortRenderer.renderVectorAsRectangles(sortController.getItems());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow* window, SortController* sortController)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (!sortController->isSorting())
	{
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			sortController->shuffleItems();
		}
		
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		{
			sortController->startSortWrapper();
		}
	}
	else
	{		
		if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
		{
			sortController->interruptSort();
		}
	}
}
