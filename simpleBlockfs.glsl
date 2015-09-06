#version 330

layout(location = 0) out vec4 outputColor;
in vec2 texCoord;
flat in int ftextureId;
flat in mat2 fglightMat;

uniform sampler2DArray blockSampler;

void main()
{
	vec2 r1 = vec2(1 - texCoord.x, texCoord.x);
	vec2 r2 = vec2(1 - texCoord.y, texCoord.y);
	float glight = dot(r1, fglightMat * r2);
	
	vec4 tex_color = texture(blockSampler, vec3(texCoord, ftextureId));
	vec3 color = vec3(1, 1, 1) * (pow(0.02, 1.0 - glight) - 0.02*(1.0 - glight));
	vec3 result = tex_color.rgb * color;
	
	outputColor = vec4(result, tex_color.w);
}
