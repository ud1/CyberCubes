#version 330
layout(location = 0) in vec2 position;
smooth out vec2 fposition;
void main()
{
	gl_Position = vec4(position, 0, 1);
	fposition = position;
}
