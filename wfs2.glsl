#version 330

layout(location = 0) out vec4 outputColor;
in vec4 viewSpace;
in vec2 relCoord;
in vec2 texCoord;
flat in vec2 fcolor;
flat in int ftextureId;
flat in mat2 fglightMat;
flat in mat2 fslightMat;

uniform float fogFar;
uniform float lightMultiplier;
uniform vec3 fogColor;

uniform sampler2DArray blockSampler;
uniform sampler2D HSColorSampler;

void main()
{
	float dist = length(viewSpace);
	float fogFactor = (fogFar - dist)/(fogFar - 3*fogFar/4);
	fogFactor = clamp( fogFactor, 0.0, 1.0 );
	
	vec2 r1 = vec2(1 - relCoord.x, relCoord.x);
	vec2 r2 = vec2(1 - relCoord.y, relCoord.y);
	float slight = dot(r1, fslightMat * r2);
	float glight = dot(r1, fglightMat * r2);
	
	vec4 tex_color = texture(blockSampler, vec3(texCoord, ftextureId - 1));
	vec3 color = texture(HSColorSampler, fcolor).rgb * (pow(0.02, 1.0 - glight) - 0.02*(1.0 - glight)) * 3.0;
	vec3 sun_color = vec3(1, 1, 1) * (pow(0.02, 1.0 - slight) - 0.02*(1.0 - slight));
	vec3 result = tex_color.rgb * max(sun_color, color) * lightMultiplier * fogFactor + (1 - fogFactor) * fogColor;
	
	outputColor = vec4(result, tex_color.w);
}
