#version 330
smooth in vec2 fposition;
out vec4 outputColor;

uniform sampler2DRect textureSampler;

void main()
{
	vec2 c = textureSize(textureSampler, 0)*(fposition.xy + vec2(1,1)) / 2;
	outputColor = vec4(texture(textureSampler, c).rgb, 1.0);
}
