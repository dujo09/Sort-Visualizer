#version 330 core

layout (location = 0) in vec2 inPosition;

out vec3 outColor;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform vec3 color;

void main()
{
	gl_Position = projectionMatrix * modelMatrix * vec4(inPosition, 0.0f, 1.0f);
	outColor = color;
};

