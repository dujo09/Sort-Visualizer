#pragma once

#include <glm/glm.hpp>


class Sortable 
{
private:
	float m_value;
	glm::vec3 m_color;
public:
	Sortable();
	Sortable(float value, glm::vec3 color = {1.0f, 1.0f, 1.0f});

	float getValue() const { return m_value; };
	glm::vec3 getColor() const { return m_color; };

	void setValue(float value) { m_value = value; };
	void setColor(glm::vec3 color) { m_color = color; };

	bool operator==(Sortable item);
	bool operator>(Sortable item);
	bool operator<(Sortable item);
};

