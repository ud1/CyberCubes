#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;
layout(location = 3) in vec4 color;

flat out vec3 fnormal;
out vec2 ftextureCoords;
flat out vec3 fcolor;

uniform mat4 MVP;

void main()
{
	fnormal = normal;
	gl_Position = MVP * vec4(position, 1.0);
	ftextureCoords = textureCoords;
	
	float s = pow(0.02, (1 - color.r)) - 0.02*(1 - color.r);
	float r = pow(0.02, (1 - color.g)) - 0.02*(1 - color.g);
	float g = pow(0.02, (1 - color.b)) - 0.02*(1 - color.b);
	float b = pow(0.02, (1 - color.a)) - 0.02*(1 - color.a);
	fcolor = max(vec3(1, 1, 1) * s, vec3(r, g, b));
}
