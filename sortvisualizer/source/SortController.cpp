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
	default:
		break;
	}

	m_isSorting = false;

	if (!m_isInterrupt)
	{
		std::cout << "Sort complete\n";
		std::cout << "Number of comparisons: " << numberOfComparisons << "\n";
		std::cout << "Number of items: " << m_items.size() << "\n";
		std::cout << "Time step: " << m_timeStepMicroseconds << " mircoseconds | " 
			<< (double)m_timeStepMicroseconds / 1000000 << " seconds\n";

		highlightItemsAsSorted();
	}
	else
	{
		std::cout << "Sort interrupted\n";
	}
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

			if (m_items[j] > m_items[j + 1])
			{
				++numberOfComparisons;
				swapItemsAndHighlight(j, j + 1);
			}
		}
	}

	return numberOfComparisons;
}

void SortController::swapItemsAndHighlight(int indexA, int indexB)
{
	Sortable temp = m_items[indexA];
	m_items[indexA] = m_items[indexB];
	m_items[indexB] = temp;

	m_items[indexA].setColor(highlightColors::RED);
	m_items[indexB].setColor(highlightColors::RED);

	std::this_thread::sleep_for(std::chrono::microseconds(m_timeStepMicroseconds));

	m_items[indexA].setColor(highlightColors::WHITE);
	m_items[indexB].setColor(highlightColors::WHITE);
}

void SortController::highlightItemsAsSorted()
{
	for (int i = 0; i < m_items.size(); ++i)
	{
		m_items[i].setColor(highlightColors::GREEN);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / m_items.size()));
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));

	for (int i = 0; i < m_items.size(); ++i)
	{
		m_items[i].setColor(highlightColors::WHITE);
	}
}
