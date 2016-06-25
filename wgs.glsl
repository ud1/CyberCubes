#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 depthBiasMVP1;
uniform mat4 depthBiasMVP2;
uniform vec3 sunLightDir;
in vec4 glight[];
in vec4 slight[];
in vec2 gcolor[];
in int gtextureId[];
in int grotationIndex[];
in int gnormalIndex[];

out vec4 viewSpace;
out vec2 relCoord;
out vec2 texCoord;
out vec4 shadowCoord1;
out vec4 shadowCoord2;
out vec3 fcoord;
flat out vec2 fcolor;
flat out int ftextureId;
flat out mat2 fglightMat;
flat out mat2 fslightMat;
flat out float fdotNormalSunLight;
flat out vec3 boxPos;

const mat2 texMat[8]  = mat2[8](mat2(1,0, 0,1), mat2(0,-1, 1,0), mat2(-1,0, 0,-1), mat2(0,1, -1,0),  mat2(-1,0, 0,1), mat2(0,-1, -1,0), mat2(1,0, 0,-1), mat2(0,1, 1,0));
const vec3 normals[6] = vec3[6](vec3(-0.5, 0, 0), vec3(0.5, 0, 0), vec3(0, -0.5, 0), vec3(0, 0.5, 0), vec3(0, 0, -0.5), vec3(0, 0, 0.5));
const vec3 t1s[6]     = vec3[6](vec3(0, 0.5, 0), vec3(0, 0.5, 0), vec3(0.5, 0, 0), vec3(0.5, 0, 0), vec3(0.5, 0, 0), vec3(0.5, 0, 0));
const vec3 t2s[6]     = vec3[6](vec3(0, 0, 0.5), vec3(0, 0, 0.5), vec3(0, 0, 0.5), vec3(0, 0, 0.5), vec3(0, 0.5, 0), vec3(0, 0.5, 0));

void main() {
	vec3 norm = normals[gnormalIndex[0]];
	fdotNormalSunLight = 2.0*dot(norm, sunLightDir);
	vec3 t1 = t1s[gnormalIndex[0]];
	vec3 t2 = t2s[gnormalIndex[0]];
	vec3 p1 = gl_in[0].gl_Position.xyz + norm;
	vec3 fposition1 = p1 - t1 - t2;
	vec3 fposition2 = p1 + t1 - t2;
	vec3 fposition3 = p1 - t1 + t2;
	vec3 fposition4 = p1 + t1 + t2;
	
	boxPos = gl_in[0].gl_Position.xyz;
	
	mat2 texMatrix = texMat[grotationIndex[0]];
	
	relCoord = vec2(0, 0);
	texCoord = texMatrix * vec2(-0.5, -0.5) + vec2(0.5, 0.5);
	fcolor = gcolor[0];
	fglightMat = mat2(glight[0]);
	fslightMat = mat2(slight[0]);
	ftextureId = gtextureId[0];
	
	gl_Position = MVP * vec4(fposition1, 1);
	shadowCoord1 = depthBiasMVP1 * vec4(fposition1,1);
	shadowCoord2 = depthBiasMVP2 * vec4(fposition1,1);
	viewSpace = MV * vec4(fposition1, 1);
	fcoord = fposition1;
	EmitVertex();

	relCoord = vec2(1, 0);
	texCoord = texMatrix * vec2(0.5, -0.5) + vec2(0.5, 0.5);
	gl_Position = MVP * vec4(fposition2, 1);
	shadowCoord1 = depthBiasMVP1 * vec4(fposition2,1);
	shadowCoord2 = depthBiasMVP2 * vec4(fposition2,1);
	viewSpace = MV * vec4(fposition2, 1);
	fcoord = fposition2;
	EmitVertex();
	
	relCoord = vec2(0, 1);
	texCoord = texMatrix * vec2(-0.5, 0.5) + vec2(0.5, 0.5);
	gl_Position = MVP * vec4(fposition3, 1);
	shadowCoord1 = depthBiasMVP1 * vec4(fposition3,1);
	shadowCoord2 = depthBiasMVP2 * vec4(fposition3,1);
	viewSpace = MV * vec4(fposition3, 1);
	fcoord = fposition3;
	EmitVertex();
	
	relCoord = vec2(1, 1);
	texCoord = texMatrix * vec2(0.5, 0.5) + vec2(0.5, 0.5);
	gl_Position = MVP * vec4(fposition4, 1);
	shadowCoord1 = depthBiasMVP1 * vec4(fposition4,1);
	shadowCoord2 = depthBiasMVP2 * vec4(fposition4,1);
	viewSpace = MV * vec4(fposition4, 1);
	fcoord = fposition4;
	EmitVertex();

	EndPrimitive();
} 
