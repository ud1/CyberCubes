#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in float textureId;
layout(location = 2) in vec4 light;
layout(location = 3) in vec4 sunLight;

uniform vec3 chunkPosition;
uniform float dayNightLightCoef;
out int gtextureId;
out vec4 glight;

uniform mat4 MVP;

void main()
{
	gl_Position = vec4(position.xyz + chunkPosition, 1);
	//gl_Position = MVP * vec4(position.xyz + chunkPosition, 1);
	//gl_Position = vec4(position.xyz, 1);
	//gl_Position = vec4(0.5, 0.5, 0, 1);
	gtextureId = int(textureId);
	glight = max(sunLight * dayNightLightCoef, light);
}
