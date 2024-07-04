#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec2 texCood;

in vec3 inPosition;
in vec3 inVertexColor;

out vec3 ourColor;
out vec2 textureCood;

void main() {
	gl_Position = vec4(position, 1.0);
	ourColor = vertexColor;
	textureCood = texCood;
	//gl_Position = vec4(inPosition, 1.0);
	//ourColor = inVertexColor;
}