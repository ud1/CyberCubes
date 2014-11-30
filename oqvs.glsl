#version 330
layout(location = 0) in vec3 position;

uniform mat4 MVP;
uniform vec3 chunkPosition;

void main()
{
  gl_Position = MVP * vec4(position + chunkPosition, 1);
}
