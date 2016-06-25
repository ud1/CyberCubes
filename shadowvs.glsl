#version 330
layout(location = 0) in vec3 pos;

uniform vec3 chunkPosition;
out vec2 gsize;
out int gnormalIndex;

void main()
{
	vec3 position = floor(pos/8);
	vec3 dims = pos - position*8;
	
	gl_Position = vec4(position + chunkPosition, 1);
	gsize = 2*dims.xy;
	gnormalIndex = int(dims.z);
}
