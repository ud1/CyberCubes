#version 330
layout(location = 0) in int normalIndex;
layout(location = 1) in int textureId;
layout(location = 2) in vec4 light;

out vec4 glight;
out int gtextureId;
out int gnormalIndex;

void main()
{
	gl_Position = vec4(0, 0, 0, 1);
	gtextureId = textureId;
	glight = light;
	gnormalIndex = normalIndex;
}

