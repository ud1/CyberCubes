#version 330

layout(location = 0) out vec4 outputColor;
in vec4 fposition;
in vec4 viewSpace;
flat in vec2 fcolor;
flat in int ftextureId;
flat in mat2 fglightMat;
flat in mat2 fslightMat;
flat in int fnormalIndex;

uniform vec3 norm;
uniform float fogFar;
uniform float lightMultiplier;
uniform vec3 fogColor;

uniform sampler2DArray blockSampler;
uniform sampler2D HSColorSampler;

void main()
{
  /*float dist = length(viewSpace);
  float fogFactor = (fogFar - dist)/(fogFar - 3*fogFar/4);
  fogFactor = clamp( fogFactor, 0.0, 1.0 );
  vec3 coord = fract(fposition.xyz + vec3(0.5, 0.5, 0.5));
  float texCount = 2;
  
  if (norm.x != 0)
    outputColor = vec4(fogFactor, coord.yz, texCount);
  else if (norm.y != 0)
    outputColor = vec4(fogFactor, coord.xz, texCount);
  else
    outputColor = vec4(fogFactor, coord.xy, texCount);
  
  outputMaterial = ftextureId;
  float slight = dot(vec2(1 - outputColor.y, outputColor.y), fslightMat * vec2(1 - outputColor.z, outputColor.z));
  float glight = dot(vec2(1 - outputColor.y, outputColor.y), fglightMat * vec2(1 - outputColor.z, outputColor.z));*/
  
	vec3 coord = fract(fposition.xyz + vec3(0.5, 0.5, 0.5));
	vec2 texCoord;
	if (fnormalIndex == 0 || fnormalIndex == 1)
		texCoord = coord.yz;
	else if (fnormalIndex == 2 || fnormalIndex == 3)
		texCoord = coord.xz;
	else
		texCoord = coord.xy;
    
	vec4 tex_color = texture(blockSampler, vec3(texCoord, ftextureId - 1));
	//tex_color = mix(tex_color, vec4(1, 1, 1, 1), 0.5);
	
	float slight = dot(vec2(1 - texCoord.x, texCoord.x), fslightMat * vec2(1 - texCoord.y, texCoord.y));
	float glight = dot(vec2(1 - texCoord.x, texCoord.x), fglightMat * vec2(1 - texCoord.y, texCoord.y));
	//glight = 0;
	vec3 color = texture(HSColorSampler, fcolor).rgb * (pow(0.02, 1.0 - glight) - 0.02*(1.0 - glight)) * 3.0;
	//vec3 color = vec3(1, 1, 1) * (pow(0.02, 1.0 - glight) - 0.02*(1.0 - glight)) * 3.0;
	
	vec3 sun_color = vec3(1, 1, 1) * (pow(0.02, 1.0 - slight) - 0.02*(1.0 - slight));
	
	float dist = length(viewSpace);
	float fogFactor = (fogFar - dist)/(fogFar - 3*fogFar/4);
	fogFactor = clamp( fogFactor, 0.0, 1.0 );
  
	vec3 result = tex_color.rgb * max(sun_color, color) * lightMultiplier * fogFactor+ (1 - fogFactor) * fogColor;
	
	outputColor = vec4(result, tex_color.w);
	//outputColor = vec4(1, 1, 0, 0.5);
}
