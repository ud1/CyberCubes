#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

uniform mat4 MVP;
uniform mat4 MV;
in vec4 glight[];
in vec4 slight[];
in vec2 gcolor[];
in int gtextureId[];
in int gnormalIndex[];

out vec4 viewSpace;
out vec2 texCoord;
flat out vec2 fcolor;
flat out int ftextureId;
flat out mat2 fglightMat;
flat out mat2 fslightMat;

const vec3 normals[6] = vec3[6](vec3(-0.5, 0, 0), vec3(0.5, 0, 0), vec3(0, -0.5, 0), vec3(0, 0.5, 0), vec3(0, 0, -0.5), vec3(0, 0, 0.5));
const vec3 t1s[6]     = vec3[6](vec3(0, 0.5, 0), vec3(0, 0.5, 0), vec3(0.5, 0, 0), vec3(0.5, 0, 0), vec3(0.5, 0, 0), vec3(0.5, 0, 0));
const vec3 t2s[6]     = vec3[6](vec3(0, 0, 0.5), vec3(0, 0, 0.5), vec3(0, 0, 0.5), vec3(0, 0, 0.5), vec3(0, 0.5, 0), vec3(0, 0.5, 0));

void main() {
	vec3 norm = normals[gnormalIndex[0]];
	vec3 t1 = t1s[gnormalIndex[0]];
	vec3 t2 = t2s[gnormalIndex[0]];
	vec3 p1 = gl_in[0].gl_Position.xyz + norm;
	vec3 fposition1 = p1 - t1 - t2;
	vec3 fposition2 = p1 + t1 - t2;
	vec3 fposition3 = p1 - t1 + t2;
	vec3 fposition4 = p1 + t1 + t2;
	
	vec4 glPos2 = MVP * vec4(fposition2, 1);
	vec4 viewSpace2 = MV * vec4(fposition2, 1);
	
	vec4 glPos3 = MVP * vec4(fposition3, 1);
	vec4 viewSpace3 = MV * vec4(fposition3, 1);

	texCoord = vec2(0, 0);
	fcolor = gcolor[0];
	fglightMat = mat2(glight[0]);
	fslightMat = mat2(slight[0]);
	ftextureId = gtextureId[0];
	
	gl_Position = MVP * vec4(fposition1, 1);
	viewSpace = MV * vec4(fposition1, 1);
	EmitVertex();

	texCoord = vec2(1, 0);
	gl_Position = glPos2;
	viewSpace = viewSpace2;
	EmitVertex();
	
	texCoord = vec2(0, 1);
	gl_Position = glPos3;
	viewSpace = viewSpace3;
	EmitVertex();
	
	texCoord = vec2(1, 1);
	gl_Position = MVP * vec4(fposition4, 1);
	viewSpace = MV * vec4(fposition4, 1);
	EmitVertex();

	EndPrimitive();
} 
