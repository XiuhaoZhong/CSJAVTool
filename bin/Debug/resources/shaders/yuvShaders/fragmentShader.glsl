#version 430

in vec2 texCoordinates;

uniform sampler2D texY;
uniform sampler2D texU;
uniform sampler2D texV;

out vec4 color;

void main() { 
	color = vec4(0.3, 0.9, 0.6, 1.0f);
	vec3 yuv;
	vec3 rgb;
	
	yuv.x = texture2D(texY, texCoordinates).r;
	yuv.y = texture2D(texU, texCoordinates).r - 0.5;
	yuv.z = texture2D(texV, texCoordinates).r - 0.5;
	
	rgb = mat3(1,              1,       1,
			   0,       -0.39465, 2.03211,
			   1.13983, -0.58060,       0) * yuv;
	
	//gl_FragColor = vec4(rgb, 1);
	
	
}