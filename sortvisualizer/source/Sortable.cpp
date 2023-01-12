#include "Sortable.h"

Sortable::Sortable()
{
	m_value = 0.0f;
	m_color = glm::vec3(1.0f);
}

Sortable::Sortable(float value, glm::vec3 color)
{
	m_value = value;
	m_color = color;
}

bool Sortable::operator==(Sortable item) 
{
	return m_value == item.m_value;
}

bool Sortable::operator>(Sortable item) 
{
	return m_value > item.m_value;
}

bool Sortable::operator<(Sortable item) 
{
	return m_value < item.m_value;
}
