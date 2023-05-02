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
#include <array>

struct Vertex
{
	glm::vec2 position;
	glm::vec3 color;
};

void processInput(GLFWwindow*& window, SortController& sortContoller);
void configureSortController(SortController& sortController);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static Vertex* createRectangle(Vertex* target, float lowerLeftX, float lowerLeftY, float width, float height, glm::vec3 color);

const float INITIAL_SCREEN_WIDTH = 800.0f;
const float INITIAL_SCREEN_HEIGHT = 600.0f;
const float DISTANCE_BETWEEN_RECTANGLES = 0.0f;

const unsigned int MAX_ITEM_COUNT = 5000;

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

	const int MAX_INDEX_COUNT = MAX_ITEM_COUNT * 6;
	const int MAX_VERTEX_COUNT = MAX_ITEM_COUNT * 4;

	unsigned int indices[MAX_INDEX_COUNT];

	int offset = 0;
	for (int i = 0; i < MAX_INDEX_COUNT; i += 6)
	{
		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;

		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;

		offset += 4;
	}

	unsigned int VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_VERTEX_COUNT * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	SortController sortController = SortController();
	configureSortController(sortController);
	
	while (!glfwWindowShouldClose(window))
	{
		processInput(window, sortController);

		int screenWidth, screenHeight;
		glfwGetWindowSize(window, &screenWidth, &screenHeight);

		const std::vector<Sortable>& items = sortController.getItems();
		const float rectangleWidth = (float)screenWidth / items.size() - DISTANCE_BETWEEN_RECTANGLES;

		std::array<Vertex, MAX_VERTEX_COUNT> vertices;
		Vertex* buffer = vertices.data();

		int indexCount = 0;
		for (int i = 0; i < items.size(); ++i)
		{
			const float rectangleHeight = items[i].getValue() / items.size() * screenHeight;

			buffer = createRectangle(buffer, (rectangleWidth + DISTANCE_BETWEEN_RECTANGLES) * i, 0.0f, rectangleWidth, rectangleHeight, items[i].getColor());
			indexCount += 6;
		}

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

		glClearColor(visualizerColors::BLACK.x, visualizerColors::BLACK.y, visualizerColors::BLACK.z, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		defaultShader.use();
		
		glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);
		defaultShader.setMat4("projectionMatrix", projectionMatrix);

		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

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
	std::cout << "Enter number of items (max is " << MAX_ITEM_COUNT << "): ";
	std::getline(std::cin, input);
	std::stringstream(input) >> numberOfItems;
	if (numberOfItems > MAX_ITEM_COUNT || numberOfItems < 0)
	{
		numberOfItems = MAX_ITEM_COUNT;
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

static Vertex* createRectangle(Vertex* target, float lowerLeftX, float lowerLeftY, float width, float height, glm::vec3 color)
{
	target->position = glm::vec2(lowerLeftX, lowerLeftY);
	target->color = color;
	++target;

	target->position = glm::vec2(lowerLeftX + width, lowerLeftY);
	target->color = color;
	++target;

	target->position = glm::vec2(lowerLeftX + width, lowerLeftY + height);
	target->color = color;
	++target;

	target->position = glm::vec2(lowerLeftX, lowerLeftY + height);
	target->color = color;
	++target;

	return target;
}