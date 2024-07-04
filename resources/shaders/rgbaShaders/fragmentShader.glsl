#version 430

in vec3 ourColor;
in vec2 textureCood;
out vec4 color;

uniform sampler2D imgTex;

void main() {
	//color = vec4(ourColor, 1.0f);
	color = texture(imgTex, textureCood);
}