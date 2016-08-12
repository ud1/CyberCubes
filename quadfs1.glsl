#version 330
smooth in vec2 fposition;
out vec4 outputColor;

uniform sampler2DRect frameBufferTexture1; // fog, color
uniform float lightMultiplier;
uniform vec3 fogColor;

void main()
{
	vec2 c = textureSize(frameBufferTexture1, 0)*(fposition.xy + vec2(1,1)) / 2;
	
	vec4 data1 = texture(frameBufferTexture1, c);
	float fogFactor = data1.a;
	vec3 tex_color = data1.rgb;
	
	vec3 result = tex_color* lightMultiplier * fogFactor + (1 - fogFactor) * fogColor;
	outputColor = vec4(result, 1.0);
}
