#version 330
smooth in vec2 fposition;
out vec4 outputColor;

uniform sampler2DRect textureSampler;
uniform isampler2DRect materialSampler;
uniform sampler2DArray blockSampler;
uniform float lightMultiplier;

void main()
{
  vec2 c = textureSize(textureSampler, 0)*(fposition.xy + vec2(1,1)) / 2;
  
  vec3 light = texture(textureSampler, c).rgb;
  int materialId = texture(materialSampler, c).r;
  
  if (materialId == 0)
    discard;
  vec3 color = texture(blockSampler, vec3(light.g, light.b, materialId - 1)).rgb;
  outputColor = vec4(color*pow(0.01, 1.0 - light.r)*lightMultiplier, 1);
}
