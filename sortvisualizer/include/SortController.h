#pragma once

#include "Sortable.h"
#include "HighlightColors.h"

#include <vector>
#include <thread>
#include <atomic>

enum SortType
{
	BUBBLE_SORT,
	EXCHANGE_SORT
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
	void interruptSort() { m_isInterrupt = true; };

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

	void swapItemsAndHighlight(int indexA, int indexB);
	void highlightItemsAsSorted();
};

