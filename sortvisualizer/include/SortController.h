#pragma once

#include "Sortable.h"
#include "HighlightColors.h"

#include <vector>
#include <thread>
#include <atomic>
#include <functional>

enum SortType
{
	BUBBLE_SORT,
	EXCHANGE_SORT,
	SELECTION_SORT,
	INSERTION_SORT,
	SHELL_SORT,
	SHELL_SORT_HIBBARD,

	QUICK_SORT,
	MERGE_SORT
};

class SortController
{
private:
	std::vector<Sortable> m_items;
	std::atomic<bool> m_isSorting = false;
	std::atomic<bool> m_isInterrupt = false;
	std::thread m_sorting;

	unsigned int m_timeStepMicroseconds = 0;
	SortType m_sortType = BUBBLE_SORT;
public:
	SortController(unsigned int numberOfItems = 100, unsigned int timeStepMicroseconds = 0, SortType sortType = BUBBLE_SORT);
	~SortController();

	void startSortWrapper();
	void startSort();
	void interruptSort();

	void generateItems(unsigned int itemCount);
	void shuffleItems();

	bool isSorting() const { return m_isSorting; };
	std::vector<Sortable> getItems() const { return m_items; };
	int getNumberOfItems() const { return m_items.size(); };

	void setTimeStep(unsigned int timeStepMicroseconds) { m_timeStepMicroseconds = timeStepMicroseconds; };
	void setSortType(SortType sortType) { m_sortType = sortType; };
private:
	int bubbleSort();
	int exchangeSort();
	int selectionSort();
	int insertionSort();

	int shellSort(std::function<int(int, int)> calculateGapSize);
	static int calculateShellGapSize(int numberOfItems, int iterationNumber);
	static int calculateHibbardGapSize(int numberOfItems, int iterationNumber);

	int quickSortWrapper();
	void quickSort(int lowIndex, int highIndex, int* numberOfComparisons);
	int partitionWithPivotAtEnd(int lowIndex, int highIndex, int* numberOfComparisons);

	void swapAndHighlightItemsAtIndices(int indexA, int indexB, const glm::vec3 highlightColor, int timeSleepHighlight);
	bool isSortedAndHighlight();
	void setAllItemsColors(glm::vec3 highlightColor);
};

