#version 330
smooth in vec2 fposition;
out vec4 outputColor;

uniform sampler2DRect frameBufferTexture1; // fog, texture coord x, y
uniform sampler2DRect frameBufferTexture2; // sun, light, hue, saturation
uniform sampler2DRect frameBufferTexture3; // ddx, ddy
uniform isampler2DRect materialSampler;
uniform sampler2DArray blockSampler;
uniform sampler2D HSColorSampler;
uniform sampler2D detailSampler;
uniform float lightMultiplier;
uniform vec3 fogColor;

void main()
{
	vec2 c = textureSize(frameBufferTexture1, 0)*(fposition.xy + vec2(1,1)) / 2;
	
	int materialId = texture(materialSampler, c).r;
	
	if (materialId == 0)
		discard;
		
	vec3 data1 = texture(frameBufferTexture1, c).rgb;
	float fogFactor = data1.r;
	vec2 texCoord = data1.gb;
	
	vec4 ddxy = texture(frameBufferTexture3, c);
	
	vec4 colorData = texture(frameBufferTexture2, c);
	vec3 sun_color = vec3(1, 1, 1) * (pow(0.02, 1.0 - colorData.y) - 0.02*(1.0 - colorData.y));
	vec3 color = texture(HSColorSampler, vec2(colorData.z, colorData.w)).rgb * (pow(0.02, 1.0 - colorData.x) - 0.02*(1.0 - colorData.x)) * 3.0;
	
	vec3 detailColor = texture(detailSampler, vec2(texCoord.x, texCoord.y)*5).rgb - vec3(0.5, 0.5, 0.5);
	vec2 shift = detailColor.rg * 0.03;
	vec3 detailColor2 = texture(detailSampler, vec2(texCoord.x, texCoord.y) + shift).rgb - vec3(0.5, 0.5, 0.5);
	vec3 tex_color = textureGrad(blockSampler, vec3(texCoord.x + shift.x, texCoord.y + shift.y, materialId - 1), ddxy.xy, ddxy.zw).rgb * (vec3(1.0, 1.0, 1.0) + detailColor2 * 0.4);
	
	vec3 result = tex_color*max(sun_color, color) * lightMultiplier * fogFactor + (1 - fogFactor) * fogColor;
	outputColor = vec4(result, 1.0);
}
