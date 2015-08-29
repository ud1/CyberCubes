#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 MVP;
uniform int normalIndex;

const vec3 normals[6] = vec3[6](vec3(-0.5, 0, 0), vec3(0.5, 0, 0), vec3(0, -0.5, 0), vec3(0, 0.5, 0), vec3(0, 0, -0.5), vec3(0, 0, 0.5));
const vec3 t1s[6]     = vec3[6](vec3(0, 0.5, 0), vec3(0, 0.5, 0), vec3(0.5, 0, 0), vec3(0.5, 0, 0), vec3(0.5, 0, 0), vec3(0.5, 0, 0));
const vec3 t2s[6]     = vec3[6](vec3(0, 0, 0.5), vec3(0, 0, 0.5), vec3(0, 0, 0.5), vec3(0, 0, 0.5), vec3(0, 0.5, 0), vec3(0, 0.5, 0));

void main() {
	vec3 norm = normals[normalIndex];
	vec3 t1 = t1s[normalIndex];
	vec3 t2 = t2s[normalIndex];
	
	vec3 p1 = gl_in[0].gl_Position.xyz + norm;
	vec3 fposition1 = p1 - t1 - t2;
	vec3 fposition2 = p1 + t1 - t2;
	vec3 fposition3 = p1 - t1 + t2;
	vec3 fposition4 = p1 + t1 + t2;
	
	gl_Position = MVP * vec4(fposition1, 1);
	EmitVertex();
	
	gl_Position = MVP * vec4(fposition2, 1);
	EmitVertex();
	
	gl_Position = MVP * vec4(fposition3, 1);
	EmitVertex();
	
	gl_Position = MVP * vec4(fposition4, 1);
	EmitVertex();
	
	EndPrimitive();
}  
