#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertexColor;

attribute vec3 inPosition;
attribute vec3 inVertexColor;

out vec3 ourColor;

void main() {
	gl_Position = vec4(position, 1.0);
	ourColor = vertexColor;
	//gl_Position = vec4(inPosition, 1.0);
	//ourColor = inVertexColor;
}