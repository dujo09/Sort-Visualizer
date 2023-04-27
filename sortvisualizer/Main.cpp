#include "SortController.h"
#include "Shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>


void processInput(GLFWwindow*& window, SortController& sortContoller);
void configureSortController(SortController& sortController);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

const float INITIAL_SCREEN_WIDTH = 800.0f;
const float INITIAL_SCREEN_HEIGHT = 600.0f;
const unsigned int MAX_NUMBER_OF_ITEMS = 1000;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "Sort Visualizer", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowSizeLimits(window, 200, 200, GLFW_DONT_CARE, GLFW_DONT_CARE);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load GLAD function\n";
		glfwTerminate();
		return -1;
	}	

	Shader defaultShader("shaders/DefaultShader.vert", "shaders/DefaultShader.frag");

	float quadVertices[] = {
		0.0f, -1.0f,
		1.0f,  0.0f,
		0.0f,  0.0f,

		0.0f, -1.0f,
		1.0f, -1.0f,
		1.0f,  0.0f,
	};

	unsigned int VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	SortController sortController = SortController();
	configureSortController(sortController);
	
	while (!glfwWindowShouldClose(window))
	{
		processInput(window, sortController);

		defaultShader.use();
		glBindVertexArray(VAO);

		glClearColor(visualizerColors::BLACK.x, visualizerColors::BLACK.y, visualizerColors::BLACK.z, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		int screenWidth, screenHeight;
		glfwGetWindowSize(window, &screenWidth, &screenHeight);

		glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
		defaultShader.setMat4("projectionMatrix", projectionMatrix);

		const std::vector<Sortable>& items = sortController.getItems();
		const float rectangleWidth = (float)screenWidth / items.size();

		for (int i = 0; i < items.size(); ++i)
		{
			const float rectangleHeight = items[i].getValue() / items.size() * screenHeight;

			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3((rectangleWidth) * i, screenHeight, 0.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(rectangleWidth, rectangleHeight, 1.0f));

			defaultShader.setMat4("modelMatrix", modelMatrix);
			defaultShader.setVec3("color", items[i].getColor());

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

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
		sortController.interruptSort();
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
	else if(sortController.isSorting())
	{		
		if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
		{
			sortController.interruptSort();
		}

		const unsigned int TIME_STEP_DELTA = 2;
		
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			sortController.setTimeStep(sortController.getTimeStep() + TIME_STEP_DELTA);
		}

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			sortController.setTimeStep(sortController.getTimeStep() - TIME_STEP_DELTA);
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
		<< "\t" << SELECTION_SORT << " - selection sort\n"
		<< "\t" << INSERTION_SORT << " - insertion sort\n"
		<< "\t" << SHELL_SORT << " - shell sort\n"
		<< "\t" << SHELL_SORT_HIBBARD << " - shell sort with hibbard gap size\n"
		<< "\t" << QUICK_SORT << " - quick sort\n"
		<< "\t" << HEAP_SORT << " - heap sort\n";
	std::cout << "Choose sort algorithm: ";
	std::getline(std::cin, input);
	std::stringstream(input) >> sortTypeOrdinal;
	if (sortTypeOrdinal < 0 || sortTypeOrdinal >= NUMBER_OF_SORTS)
	{
		sortTypeOrdinal = 0;
	}

	sortController.generateItems(numberOfItems);
	sortController.shuffleItems();
	sortController.setTimeStep(timeStepMicroseconds);
	sortController.setSortType(SortType(sortTypeOrdinal));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}