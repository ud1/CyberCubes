#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in int normalIndex;
layout(location = 2) in vec2 color;
layout(location = 3) in int rotationIndex;
layout(location = 4) in int textureId;
layout(location = 5) in vec4 light;
layout(location = 6) in vec4 sunLight;

uniform vec3 chunkPosition;
uniform float dayNightLightCoef;

out vec4 glight;
out vec4 slight;
out vec2 gcolor;
out int grotationIndex;
out int gtextureId;
out int gnormalIndex;

void main()
{
	gl_Position = vec4(position.xyz + chunkPosition, 1);
	grotationIndex = rotationIndex;
	gtextureId = textureId;
	slight = sunLight * dayNightLightCoef;
	glight = light;
	gcolor = color;
	gnormalIndex = normalIndex;
}
