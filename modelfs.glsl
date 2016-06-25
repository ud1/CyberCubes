#version 330
layout(location = 0) out vec4 outputColor;

flat in vec3 fnormal; 

void main()
{
	outputColor = vec4((fnormal + vec3(1.0f, 1.0f, 1.0f))*0.5, 1.0f);
}
