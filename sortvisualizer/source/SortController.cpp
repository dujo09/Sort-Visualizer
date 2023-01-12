#include "SortController.h"

#include <ranges>
#include <random>
#include <algorithm>
#include <numeric>


SortController::SortController()
{
	m_items = std::vector<Sortable>(50);
}

SortController::~SortController()
{
	m_timeStepMiliseconds = 0;
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
		// highlight last item of current loop
		m_items[m_items.size() - i - 1].setColor(highlightColors::YELLOW);

		for (int j = 0; j < m_items.size() - i - 1; j++)
		{
			// highlight current item
			m_items[j].setColor(highlightColors::RED);
			// wait between each step
			std::this_thread::sleep_for(std::chrono::milliseconds(m_timeStepMiliseconds));

			if (m_items[j] > m_items[j + 1])
			{
				// highlight item being swapped
				m_items[j + 1].setColor(highlightColors::RED);

				Sortable temp = m_items[j];
				m_items[j] = m_items[j + 1];
				m_items[j + 1] = temp;

				// wait an additional interval to emphasize swapping
				std::this_thread::sleep_for(std::chrono::milliseconds(m_timeStepMiliseconds));
			}
			// unhighlight current item
			m_items[j].setColor(highlightColors::WHITE);
			// note: no need to unhighlight item at j + 1 since it will be highlighted in next loop
		}
		// unhighlight last item of current loop
		m_items[m_items.size() - i - 1].setColor(highlightColors::WHITE);
	}

	m_isSorting = false;
	return;
}
