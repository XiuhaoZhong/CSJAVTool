#version 430

in vec3 renderPosition;
in vec2 texPos;

out vec2 texCoordinates;

void main() {
	//texCoordinates = vec2(texPos.x, texPos.y);
	texCoordinates = texPos;
	gl_Position = vec4(renderPosition, 1.0);	
}