#version 330 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec3 inColor;

out vec3 aColor;

uniform mat4 projectionMatrix;

void main()
{
	gl_Position = projectionMatrix * vec4(inPosition, 0.0f, 1.0f);
	aColor = inColor;
};

