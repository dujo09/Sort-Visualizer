#include "SortController.h"

#include <ranges>
#include <random>
#include <algorithm>
#include <numeric>
#include <iostream>


SortController::SortController(const unsigned int numberOfItems, const unsigned int timeStepMicroseconds, const SortType sortType):
	m_timeStepMicroseconds(timeStepMicroseconds), m_sortType(sortType)
{
	generateItems(numberOfItems);
	shuffleItems();
}

SortController::~SortController()
{
	m_isInterrupt = true;
	if (m_sorting.joinable())
	{
		m_sorting.join();
	}
}

void SortController::startSortWrapper()
{
	if (m_sorting.joinable())
	{
		m_sorting.join();
	}
	m_sorting = std::thread(&SortController::startSort, this);
}

void SortController::startSort()
{
	std::cout << "\nSorting...\n";

	int numberOfComparisons = 0;

	m_isSorting = true;
	m_isInterrupt = false;

	switch (m_sortType)
	{
	case BUBBLE_SORT:
		numberOfComparisons = bubbleSort();
		break;
	case EXCHANGE_SORT:
		numberOfComparisons = exchangeSort();
		break;
	case SELECTION_SORT:
		numberOfComparisons = selectionSort();
		break;
	case INSERTION_SORT:
		numberOfComparisons = insertionSort();
		break;
	case SHELL_SORT:
		numberOfComparisons = shellSort(&calculateShellGapSize);
		break;
	case SHELL_SORT_HIBBARD:
		numberOfComparisons = shellSort(&calculateHibbardGapSize);
		break;
	case QUICK_SORT:
		numberOfComparisons = quickSortWrapper();
		break;
	case HEAP_SORT:
		numberOfComparisons = heapSort();
		break;
	default:
		break;
	}

	if (m_isInterrupt)
	{
		std::cout << "Sort interrupted\n";
	}
	else
	{
		if (isSorted())
		{
			std::cout << "Sort complete\n";
			std::cout << "Number of comparisons: " << numberOfComparisons << "\n";
			std::cout << "Number of items: " << m_items.size() << "\n";
			std::cout << "Time step: " << m_timeStepMicroseconds << " mircoseconds\n";

			highlightItemsAsSorted();
		}
		else
		{
			std::cout << "Error items were not sorted\n";
			setItemsColors(visualizerColors::RED);
		}
	}

	setItemsColors(visualizerColors::WHITE);

	m_isSorting = false;
}

void SortController::interruptSort()
{
	m_isInterrupt = true;
}

void SortController::generateItems(unsigned int itemCount)
{
	m_items.clear();
	for (int i = 1; i <= itemCount; ++i)
	{
		m_items.push_back(i);
	}
}

void SortController::shuffleItems()
{
	std::shuffle(m_items.begin(), m_items.end(), std::default_random_engine());
}

void SortController::setTimeStep(int timeStepMicroseconds)
{
	if (timeStepMicroseconds <= 0)
	{
		return;
	}
	m_timeStepMicroseconds = timeStepMicroseconds;
}

int SortController::bubbleSort()
{
	int numberOfComparisons = 0;
	for (int i = 0; i < m_items.size() - 1; i++)
	{
		for (int j = 0; j < m_items.size() - i - 1; j++)
		{	
			if (m_isInterrupt)
			{
				return 0;
			}

			++numberOfComparisons;
			if (m_items[j] > m_items[j + 1])
			{
				swapAndHighlightItemsAtIndices(j, j + 1, visualizerColors::RED);
			}
		}
	}
	return numberOfComparisons;
}

int SortController::exchangeSort()
{
	int numberOfComparisons = 0;
	for (int i = 0; i < m_items.size() - 1; i++)
	{
		for (int j = i + 1; j < m_items.size(); j++)
		{
			if (m_isInterrupt)
			{
				return 0;
			}

			++numberOfComparisons;
			if (m_items[i] > m_items[j])
			{
				swapAndHighlightItemsAtIndices(i, j, visualizerColors::RED);
			}
		}
	}
	return numberOfComparisons;
}

int SortController::selectionSort()
{
	int numberOfComparisons = 0;

	int minIndex = 0;
	for (int i = 0; i < m_items.size() - 1; i++)
	{
		minIndex = i;
		for (int j = i + 1; j < m_items.size(); j++)
		{
			if (m_isInterrupt)
			{
				return 0;
			}

			m_items[j].setColor(visualizerColors::RED);
			m_items[minIndex].setColor(visualizerColors::RED);
			std::this_thread::sleep_for(std::chrono::microseconds(m_timeStepMicroseconds));

			++numberOfComparisons;
			if (m_items[j] < m_items[minIndex])
			{
				m_items[minIndex].setColor(visualizerColors::WHITE);
				minIndex = j;
			}

			m_items[j].setColor(visualizerColors::WHITE);
			m_items[minIndex].setColor(visualizerColors::WHITE);
		}

		if (minIndex != i)
		{
			swapAndHighlightItemsAtIndices(i, minIndex, visualizerColors::RED);
		}
	}
	return numberOfComparisons;
}

int SortController::insertionSort()
{
	int numberOfComparisons = 0;
	for (int keyIndex = 1; keyIndex < m_items.size(); keyIndex++)
	{
		Sortable keyItem = m_items[keyIndex];

		int i = keyIndex - 1;
		for (; i >= 0; --i)
		{
			if (m_isInterrupt)
			{
				return 0;
			}

			++numberOfComparisons;
			if (keyItem < m_items[i])
			{
				m_items[i + 1] = m_items[i];
				m_items[i] = keyItem; // done here in order to better visualize insertion

				m_items[i].setColor(visualizerColors::RED);
				std::this_thread::sleep_for(std::chrono::microseconds(m_timeStepMicroseconds));
				m_items[i].setColor(visualizerColors::WHITE);
			}
			else
			{
				break;
			}
		}

	}
	return numberOfComparisons;
}

int SortController::shellSort(std::function<int(int, int)> calculateGapSize)
{
	int numberOfComparisons = 0, iterationNumber = 0, gapSize = 0;

	while ((gapSize = calculateGapSize(m_items.size(), iterationNumber)) > 0)
	{
		++iterationNumber;

		for (int i = gapSize; i < m_items.size(); i += 1)
		{
			if (m_isInterrupt)
			{
				return 0;
			}

			Sortable keyItem = m_items[i];

			for (int j = i; j >= gapSize && m_items[j - gapSize] > keyItem; j -= gapSize)
			{
				m_items[j] = m_items[j - gapSize];
				m_items[j - gapSize] = keyItem; // done here in order to better visualize insertion

				m_items[j].setColor(visualizerColors::RED);
				m_items[j - gapSize].setColor(visualizerColors::RED);
				std::this_thread::sleep_for(std::chrono::microseconds(m_timeStepMicroseconds));
				m_items[j].setColor(visualizerColors::WHITE);
				m_items[j - gapSize].setColor(visualizerColors::WHITE);
			}
		}
	}
	return numberOfComparisons;
}

int SortController::calculateShellGapSize(int numberOfItems, int iterationNumber)
{
	return numberOfItems / pow(2, iterationNumber + 1);
}

int SortController::calculateHibbardGapSize(int numberOfItems, int iterationNumber)
{
	const int maxNumberOfIterations = log(numberOfItems + 1) / log(2);
	return pow(2, maxNumberOfIterations - iterationNumber) - 1;;
}

int SortController::quickSortWrapper()
{
	int numberOfComparisons = 0;

	quickSort(0, m_items.size() - 1, &numberOfComparisons);

	return numberOfComparisons;
}

void SortController::quickSort(int lowIndex, int highIndex, int* numberOfComparisons)
{
	if (m_isInterrupt)
	{
		*numberOfComparisons = 0;
		return;
	}

	if (lowIndex < highIndex)
	{
		int pivotIndex = partitionWithPivotAtEnd(lowIndex, highIndex, numberOfComparisons);

		quickSort(lowIndex, pivotIndex - 1, numberOfComparisons);
		quickSort(pivotIndex + 1, highIndex, numberOfComparisons);
	}

	return;
}

int SortController::partitionWithPivotAtEnd(int lowIndex, int highIndex, int* numberOfComparisons)
{
	Sortable pivotItem = m_items[highIndex];
	m_items[highIndex].setColor(visualizerColors::HIGHLIGHT_COLOR_TWO);

	int i = lowIndex - 1;

	for (int j = lowIndex; j < highIndex; j++)
	{
		if (m_isInterrupt)
		{
			return 0;
		}
		
		m_items[j].setColor(visualizerColors::HIGHLIGHT_COLOR_ONE);
		std::this_thread::sleep_for(std::chrono::microseconds(m_timeStepMicroseconds));

		++(*numberOfComparisons);
		if (m_items[j] < pivotItem)
		{
			if(i != -1)
			{
				m_items[i].setColor(visualizerColors::WHITE);
			}
			++i;
			swapAndHighlightItemsAtIndices(i, j, visualizerColors::RED);
			m_items[i].setColor(visualizerColors::HIGHLIGHT_COLOR_THREE);
		}
		m_items[j].setColor(visualizerColors::WHITE);
	}

	m_items[highIndex].setColor(visualizerColors::WHITE);
	if (i != -1)
	{
		m_items[i].setColor(visualizerColors::WHITE);
	}

	swapAndHighlightItemsAtIndices(i + 1, highIndex, visualizerColors::RED);
	return (i + 1);
}

int SortController::maxHeapify(int heapSize, int index)
{
	if (m_isInterrupt)
	{
		return 0;
	}

	int leftChildIndex = index * 2 + 1;
	int rightChildIndex = index * 2 + 2;
	int largestIndex = index;

	if (leftChildIndex < heapSize && m_items[leftChildIndex] > m_items[index])
	{
		largestIndex = leftChildIndex;
	}
	if (rightChildIndex < heapSize && m_items[rightChildIndex] > m_items[largestIndex])
	{
		largestIndex = rightChildIndex;
	}

	if (largestIndex != index)
	{
		swapAndHighlightItemsAtIndices(index, largestIndex, visualizerColors::RED);
		return 2 + maxHeapify(heapSize, largestIndex);
	}
	// return number of comparisons
	return 2;
}

int SortController::buildMaxHeap()
{
	int numberOfComparisons = 0;
	int heapSize = m_items.size();

	for (int i = heapSize / 2; i >= 0; --i)
	{
		if (m_isInterrupt)
		{
			return 0;
		}

		numberOfComparisons += maxHeapify(heapSize, i);
	}
	return numberOfComparisons;
}

int SortController::heapSort()
{
	int numberOfComparisons = 0;
	int heapSize = m_items.size();

	numberOfComparisons = buildMaxHeap();

	for (int i = heapSize - 1; i > 0; --i)
	{
		if (m_isInterrupt)
		{
			return 0;
		}

		swapAndHighlightItemsAtIndices(0, i, visualizerColors::GREEN);
		--heapSize;
		numberOfComparisons += maxHeapify(heapSize, 0);
	}

	return numberOfComparisons;
}

void SortController::swapAndHighlightItemsAtIndices(int indexA, int indexB, const glm::vec3 highlightColor)
{
	glm::vec3 colorA = m_items[indexA].getColor();
	glm::vec3 colorB = m_items[indexB].getColor();

	m_items[indexA].setColor(highlightColor);
	m_items[indexB].setColor(highlightColor);
	std::this_thread::sleep_for(std::chrono::microseconds(m_timeStepMicroseconds));
	m_items[indexA].setColor(colorA);
	m_items[indexB].setColor(colorB);

	Sortable temp = m_items[indexA];
	m_items[indexA] = m_items[indexB];
	m_items[indexB] = temp;
}

bool SortController::isSorted()
{
	for (int i = 1; i < m_items.size(); ++i)
	{
		if (m_items[i] < m_items[i - 1])
		{
			return false;
		}
	}

	return true;
}

void SortController::setItemsColors(glm::vec3 highlightColor)
{	
	for (int i = 0; i < m_items.size(); ++i)
	{
		m_items[i].setColor(highlightColor);
	}
	
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void SortController::highlightItemsAsSorted()
{
	const unsigned int TIME_STEP = 2000 / m_items.size();

	for (int i = 0; i < m_items.size(); ++i)
	{
		m_items[i].setColor(visualizerColors::RED);
		if (!m_isInterrupt)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(TIME_STEP));
		}
		m_items[i].setColor(visualizerColors::GREEN);
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));
}
