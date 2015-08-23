#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in int normalIndex;
layout(location = 2) in vec2 color;
layout(location = 3) in int textureId;
layout(location = 4) in vec4 light;
layout(location = 5) in vec4 sunLight;

uniform vec3 chunkPosition;
uniform float dayNightLightCoef;
out int gtextureId;
out vec4 glight;
out vec4 slight;
out vec2 gcolor;
out int gnormalIndex;

uniform mat4 MVP;

void main()
{
	gl_Position = vec4(position.xyz + chunkPosition, 1);
	gtextureId = int(textureId);
	slight = sunLight * dayNightLightCoef;
	glight = light;
	gcolor = color;
	gnormalIndex = normalIndex;
}
