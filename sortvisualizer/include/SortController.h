#pragma once

#include "Sortable.h"
#include "HighlightColors.h"

#include <vector>
#include <thread>
#include <atomic>

enum SortType
{
	BUBBLE_SORT
};

class SortController
{
private:
	std::vector<Sortable> m_items;
	std::atomic<bool> m_isSorting = false;
	std::thread m_sorting;

	unsigned int m_timeStepMiliseconds = 500;
	SortType m_sortType = BUBBLE_SORT;
public:
	SortController();
	~SortController();

	void startSort();

	// vector controll
	void generateItems(unsigned int itemCount);
	void shuffleItems();

	// getters
	bool isSorting() const { return m_isSorting; };
	std::vector<Sortable> getItems() const { return m_items; };
	int getNumberOfItems() const { return m_items.size(); };

	// setters
	void setTimeStep(unsigned int timeStepMiliseconds) { m_timeStepMiliseconds = timeStepMiliseconds; };
	void setSortType(SortType sortType) { m_sortType = sortType; };
private:
	void bubbleSort();

};

