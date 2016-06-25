#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 depthMVP;
in int gnormalIndex[];
in vec2 gsize[];

flat out vec3 minBoxPos;
flat out vec3 maxBoxPos;
out vec3 fcoord;

const vec3 normals[6] = vec3[6](vec3(-0.5, 0, 0), vec3(0.5, 0, 0), vec3(0, -0.5, 0), vec3(0, 0.5, 0), vec3(0, 0, -0.5), vec3(0, 0, 0.5));
const vec3 t1s[6]     = vec3[6](vec3(0, 0.5, 0), vec3(0, 0.5, 0), vec3(0.5, 0, 0), vec3(0.5, 0, 0), vec3(0.5, 0, 0), vec3(0.5, 0, 0));
const vec3 t2s[6]     = vec3[6](vec3(0, 0, 0.5), vec3(0, 0, 0.5), vec3(0, 0, 0.5), vec3(0, 0, 0.5), vec3(0, 0.5, 0), vec3(0, 0.5, 0));

void main() {
	vec3 norm = normals[gnormalIndex[0]];
	vec3 t1 = t1s[gnormalIndex[0]];
	vec3 t2 = t2s[gnormalIndex[0]];
	vec3 p1 = gl_in[0].gl_Position.xyz + norm;
	vec3 fposition1 = p1 - t1 - t2;
	vec3 fposition2 = p1 + t1*(gsize[0].x + 1) - t2;
	vec3 fposition3 = p1 - t1 + t2*(gsize[0].y + 1);
	vec3 fposition4 = p1 + t1*(gsize[0].x + 1) + t2*(gsize[0].y + 1);
	
	minBoxPos = gl_in[0].gl_Position.xyz;
	maxBoxPos = minBoxPos + t1*gsize[0].x + t2*gsize[0].y;
	
	gl_Position = depthMVP * vec4(fposition1, 1);
	fcoord = fposition1;
	EmitVertex();

	gl_Position = depthMVP * vec4(fposition2, 1);
	fcoord = fposition2;
	EmitVertex();
	
	gl_Position = depthMVP * vec4(fposition3, 1);
	fcoord = fposition3;
	EmitVertex();
	
	gl_Position = depthMVP * vec4(fposition4, 1);
	fcoord = fposition4;
	EmitVertex();

	EndPrimitive();
} 
