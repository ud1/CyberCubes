#version 330

layout(location = 0) out vec4 outputData1;
layout(location = 1) out vec4 outputData2;
layout(location = 2) out vec4 outputData3;
layout(location = 3) out int outputMaterial;
in vec4 viewSpace;
in vec2 relCoord;
in vec2 texCoord;
flat in vec2 fcolor;
flat in int ftextureId;
flat in mat2 fglightMat;
flat in mat2 fslightMat;

uniform float fogFar;

void main()
{
	float dist = length(viewSpace);
	float fogFactor = (fogFar - dist)/(fogFar - 3*fogFar/4);
	fogFactor = clamp( fogFactor, 0.0, 1.0 );
	
	vec2 r1 = vec2(1 - relCoord.x, relCoord.x);
	vec2 r2 = vec2(1 - relCoord.y, relCoord.y);
	float slight = dot(r1, fslightMat * r2);
	float glight = dot(r1, fglightMat * r2);
	
	outputData1 = vec4(fogFactor, texCoord, 0);
	outputData2 = vec4(glight, slight, fcolor);
	outputData3 = vec4(dFdx(texCoord), dFdy(texCoord));
	outputMaterial = ftextureId;
}
