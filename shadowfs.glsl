#version 330 core

layout(location = 0) out vec3 boxPosition;
flat in vec3 minBoxPos;
flat in vec3 maxBoxPos;
in vec3 fcoord;
uniform int mode;

void main()
{
	if (mode == 0)
		boxPosition = fcoord;
	else
		boxPosition = clamp(round(fcoord), minBoxPos, maxBoxPos);
}
