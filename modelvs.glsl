#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;

flat out vec3 fnormal;

uniform mat4 MVP;
uniform mat4 MV;
uniform vec3 chunkPosition;

void main()
{
	fnormal = normal;
	gl_Position = MVP * vec4(position + chunkPosition, 1.0);
}
