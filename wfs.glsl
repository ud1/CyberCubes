#version 330

layout(location = 0) out vec4 outputColor;
layout(location = 1) out vec4 outputLight;
layout(location = 2) out int outputMaterial;
in vec4 fposition;
in vec4 viewSpace;
flat in vec2 fcolor;
flat in int ftextureId;
flat in mat2 fglightMat;
flat in mat2 fslightMat;
flat in int fnormalIndex;

uniform vec3 norm;
uniform float fogFar;

void main()
{
  float dist = length(viewSpace);
  float fogFactor = (fogFar - dist)/(fogFar - 3*fogFar/4);
  fogFactor = clamp( fogFactor, 0.0, 1.0 );
  vec3 coord = fract(fposition.xyz + vec3(0.5, 0.5, 0.5));
  float texCount = 2;
  
  if (fnormalIndex == 0 || fnormalIndex == 1)
    outputColor = vec4(fogFactor, coord.yz, texCount);
  else if (fnormalIndex == 2 || fnormalIndex == 3)
    outputColor = vec4(fogFactor, coord.xz, texCount);
  else
    outputColor = vec4(fogFactor, coord.xy, texCount);
  
  outputMaterial = ftextureId;
  float slight = dot(vec2(1 - outputColor.y, outputColor.y), fslightMat * vec2(1 - outputColor.z, outputColor.z));
  float glight = dot(vec2(1 - outputColor.y, outputColor.y), fglightMat * vec2(1 - outputColor.z, outputColor.z));
  outputLight = vec4(glight, slight, fcolor);
}
