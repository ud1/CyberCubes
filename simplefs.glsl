#version 330 core

// Ouput data
layout(location = 0) out vec4 color;

uniform sampler2D shadow_map_texture;

in vec2 UV;

void main()
{
	//float c = fract(texture(shadow_map_texture, UV).z);
	float c = texture(shadow_map_texture, UV).x;
	color = vec4(c, c, c, 1);
}
