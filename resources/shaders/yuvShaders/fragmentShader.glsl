#version 430

in vec2 texCoordinates;

uniform sampler2D texY;
uniform sampler2D texU;
uniform sampler2D texV;

out vec4 color;

void main() { 
	vec3 yuv = vec3(0.34, 0.02, 0.04);
	vec3 rgb;
	
	yuv.x = texture(texY, texCoordinates).r;
	yuv.y = texture(texU, texCoordinates).r - 0.5;
	yuv.z = texture(texV, texCoordinates).r - 0.5;
	
	rgb = mat3(1,              1,       1,
			   0,       -0.39465, 2.03211,
			   1.13983, -0.58060,       0) * yuv;
	
	color = vec4(rgb, 1);
}