#include "SortController.h"

#include <ranges>
#include <random>
#include <algorithm>
#include <numeric>


SortController::SortController(const unsigned int numberOfItems, const unsigned int timeStepMicroseconds, const SortType sortType):
	m_timeStepMicroseconds(timeStepMicroseconds), m_sortType(sortType)
{
	generateItems(numberOfItems);
	shuffleItems();
}

SortController::~SortController()
{
	m_timeStepMicroseconds = 0;
	if (m_sorting.joinable())
	{
		m_sorting.join();
	}
}

void SortController::startSort()
{
	if (m_sorting.joinable())
	{
		m_sorting.join();
	}

	m_isSorting = true;

	switch (m_sortType)
	{
	case BUBBLE_SORT:
		m_sorting = std::thread(&SortController::bubbleSort, this);
		break;
	default:
		break;
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

void SortController::bubbleSort()
{
	for (int i = 0; i < m_items.size() - 1; i++)
	{
		for (int j = 0; j < m_items.size() - i - 1; j++)
		{	
			if (m_items[j] > m_items[j + 1])
			{
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

	highlightItemsAsSorted();

	m_isSorting = false;
	return;
}

void SortController::highlightItemsAsSorted()
{
	for (int i = 0; i < m_items.size(); ++i)
	{
		m_items[i].setColor(highlightColors::GREEN);
		std::this_thread::sleep_for(std::chrono::microseconds(m_timeStepMicroseconds));
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));

	for (int i = 0; i < m_items.size(); ++i)
	{
		m_items[i].setColor(highlightColors::WHITE);
	}
}
