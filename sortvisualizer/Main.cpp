#include "SortController.h"
#include "Shader.h"
#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <sstream>


void processInput(GLFWwindow*& window, SortController& sortContoller);
void configureSortController(SortController& sortController);

const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGHT = 600.0f;
const unsigned int MAX_NUMBER_OF_ITEMS = 1000;

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

	SortController sortController = SortController();
	configureSortController(sortController);
	Renderer sortRenderer = Renderer("shaders/DefaultShader.vert", "shaders/DefaultShader.frag", SCREEN_WIDTH, SCREEN_HEIGHT);
	
	while (!glfwWindowShouldClose(window))
	{
		processInput(window, sortController);

		sortRenderer.renderVectorAsRectangles(sortController.getItems());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow*& window, SortController& sortController)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (!sortController.isSorting())
	{
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			sortController.shuffleItems();
		}
		
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		{
			sortController.startSortWrapper();
		}

		if (glfwGetKey(window, GLFW_KEY_INSERT) == GLFW_PRESS)
		{
			configureSortController(sortController);
		}
	}
	else
	{		
		if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
		{
			sortController.interruptSort();
		}
	}
}

void configureSortController(SortController& sortController)
{
	std::string input;
	int numberOfItems = 0;
	int timeStepMicroseconds = 0;
	int sortTypeOrdinal = 0;

	std::cout << "\nSORT CONFIGURATION\n";
	std::cout << "Enter number of items (max is " << MAX_NUMBER_OF_ITEMS << "): ";
	std::getline(std::cin, input);
	std::stringstream(input) >> numberOfItems;
	if (numberOfItems > MAX_NUMBER_OF_ITEMS || numberOfItems < 0)
	{
		numberOfItems = MAX_NUMBER_OF_ITEMS;
	}

	std::cout << "Enter time step (in microseconds): ";
	std::getline(std::cin, input);
	std::stringstream(input) >> timeStepMicroseconds;
	if (timeStepMicroseconds < 0)
	{
		timeStepMicroseconds = 0;
	}

	std::cout << "Sort algorithms avaliable: \n" 
		<< "\t" << BUBBLE_SORT << " - bubble sort\n"
		<< "\t" << EXCHANGE_SORT << " - exchange sort\n"
		<< "\t" << SELECTION_SORT << " - selection sort\n";
	std::cout << "Choose sort algorithm: ";
	std::getline(std::cin, input);
	std::stringstream(input) >> sortTypeOrdinal;
	if (sortTypeOrdinal < 0)
	{
		sortTypeOrdinal = 0;
	}

	sortController.generateItems(numberOfItems);
	sortController.shuffleItems();
	sortController.setTimeStep(timeStepMicroseconds);
	sortController.setSortType(SortType(sortTypeOrdinal));
}
