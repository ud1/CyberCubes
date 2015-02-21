#version 330
smooth in vec2 fposition;
out vec4 outputColor;

uniform sampler2DRect textureSampler;
uniform sampler2DRect lightSampler;
uniform isampler2DRect materialSampler;
uniform sampler2DArray blockSampler;
uniform sampler2D HSColorSampler;
uniform float lightMultiplier;

void main()
{
  vec2 c = textureSize(textureSampler, 0)*(fposition.xy + vec2(1,1)) / 2;
  
  vec3 light = texture(textureSampler, c).rgb;
  int materialId = texture(materialSampler, c).r;
  
  if (materialId == 0)
    discard;
  
  vec4 colorData = texture(lightSampler, c);
  float v = max(colorData.x, colorData.y);
  vec3 sun_color = vec3(1, 1, 1) * (pow(0.05, 1.0 - colorData.y) - 0.05*(1.0 - colorData.y));
  vec3 color = texture(HSColorSampler, vec2(colorData.z, colorData.w)).rgb * (pow(0.05, 1.0 - colorData.x) - 0.05*(1.0 - colorData.x)) * 3.0;
  
  vec3 tex_color = texture(blockSampler, vec3(light.g, light.b, materialId - 1)).rgb;
  outputColor = vec4(tex_color*max(sun_color, color)*lightMultiplier, 1);
  
  //vec3 col = texture(HSColorSampler, 0.5).rgb;
  //vec3 col = texture(lightSampler, c).rgb;
  //vec3 c2 = texture(HSColorSampler, colorData.z).rgb;
  //outputColor = vec4(c2, 1);
}
