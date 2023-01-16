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
	std::cout << "Sorting...\n";

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
			<< (double)m_timeStepMicroseconds / 1000000 << " seconds\n\n";

		highlightItemsAsSorted();
	}
	else
	{
		std::cout << "Sort interrupted\n\n";
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
	unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
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
				Sortable temp = m_items[j];
				m_items[j] = m_items[j + 1];
				m_items[j + 1] = temp;

				// highlight items being swapped
				m_items[j + 1].setColor(highlightColors::RED);
				m_items[j].setColor(highlightColors::RED);
				// wait an interval to emphasize swapping
				std::this_thread::sleep_for(std::chrono::microseconds(m_timeStepMicroseconds));
				// unhighlight items
				m_items[j].setColor(highlightColors::WHITE);
				m_items[j + 1].setColor(highlightColors::WHITE);
			}
		}
	}
	return numberOfComparisons;
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
