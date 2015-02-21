#version 330

layout(location = 0) out vec4 outputColor;
layout(location = 1) out vec4 outputLight;
layout(location = 2) out int outputMaterial;
in vec4 fposition;
in vec4 flight;
flat in int ftextureId;

uniform vec3 norm;

void main()
{
  float lightValue = fposition.w;
  vec3 coord = fract(fposition.xyz + vec3(0.5, 0.5, 0.5));
  float texCount = 2;
  
  if (norm.x != 0)
    outputColor = vec4(lightValue, coord.yz, texCount);
  else if (norm.y != 0)
    outputColor = vec4(lightValue, coord.xz, texCount);
  else
    outputColor = vec4(lightValue, coord.xy, texCount);
  
  outputMaterial = ftextureId;
  outputLight = flight;
}
