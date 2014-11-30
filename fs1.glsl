#version 330
smooth in vec4 fposition;
in float ftextureId;
out vec3 outputColor;
uniform vec3 eye;
uniform vec3 norm;

vec3 lightDir = vec3(1, 1, -1);
void main()
{
  //float lightValue = clamp(dot(normalize(eye - fposition), norm), 0, 1);
  float lightValue = fposition.w;
  vec3 coord = fract(fposition.xyz + vec3(0.5, 0.5, 0.5));
  coord = 0.25 + coord/2.0;
  if (norm.x != 0)
    outputColor = vec3(lightValue, coord.yz);
  else if (norm.y != 0)
    outputColor = vec3(lightValue, coord.xz);
  else
    outputColor = vec3(lightValue, coord.xy);
  outputColor.y = (outputColor.y + ftextureId)/2;
}
