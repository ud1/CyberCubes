#version 330
layout(location = 0) in vec4 position;
layout(location = 1) in float textureId;

uniform mat4 MVP;
uniform vec3 chunkPosition;

smooth out vec4 fposition;
out float ftextureId;
void main()
{
	gl_Position = MVP * vec4(position.xyz + chunkPosition, 1);
	fposition = position;
	ftextureId = textureId;
}
