#version 330
smooth in vec2 fposition;
out vec4 outputColor;

uniform sampler2DRect textureSampler;

void main()
{
	vec2 c = textureSize(textureSampler, 0)*(fposition.xy + vec2(1,1)) / 2;
	vec3 hdrColor = texture(textureSampler, c).rgb;
	
    // reinhard
    //vec3 result = hdrColor / (hdrColor + vec3(1.0));
    // exposure
    /*const float gamma = 2.2;
    float exposure = 1.5;
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);*/
    // also gamma correct while we're at it       
    //result = pow(result, vec3(1.0 / gamma));
    //outputColor = vec4(result, 1.0);
    
    outputColor = vec4(hdrColor, 1.0);
}
