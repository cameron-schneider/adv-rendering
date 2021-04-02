#version 450

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

layout (location = 0) out vec3 outColor;

void main()
{
	//output the position of each vertex
	gl_Position = vec4(aPosition, 1.0f);

	outColor = aColor;
}