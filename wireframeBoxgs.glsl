#version 330

layout(points) in;
layout(line_strip, max_vertices = 24) out;

uniform mat4 MVP;

void main() {
  float sz = 0.505;
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(+sz, +sz, +sz), 1);
  EmitVertex();
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(+sz, +sz, -sz), 1);
  EmitVertex();
  EndPrimitive();
  
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(+sz, -sz, +sz), 1);
  EmitVertex();
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(+sz, -sz, -sz), 1);
  EmitVertex();
  EndPrimitive();
  
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(-sz, -sz, +sz), 1);
  EmitVertex();
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(-sz, -sz, -sz), 1);
  EmitVertex();
  EndPrimitive();
  
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(-sz, +sz, +sz), 1);
  EmitVertex();
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(-sz, +sz, -sz), 1);
  EmitVertex();
  EndPrimitive();
  
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(+sz, +sz, +sz), 1);
  EmitVertex();
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(+sz, -sz, +sz), 1);
  EmitVertex();
  EndPrimitive();
  
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(+sz, +sz, -sz), 1);
  EmitVertex();
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(+sz, -sz, -sz), 1);
  EmitVertex();
  EndPrimitive();
  
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(-sz, +sz, -sz), 1);
  EmitVertex();
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(-sz, -sz, -sz), 1);
  EmitVertex();
  EndPrimitive();
  
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(-sz, +sz, +sz), 1);
  EmitVertex();
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(-sz, -sz, +sz), 1);
  EmitVertex();
  EndPrimitive();
  
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(+sz, +sz, +sz), 1);
  EmitVertex();
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(-sz, +sz, +sz), 1);
  EmitVertex();
  EndPrimitive();
  
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(+sz, +sz, -sz), 1);
  EmitVertex();
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(-sz, +sz, -sz), 1);
  EmitVertex();
  EndPrimitive();
  
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(+sz, -sz, -sz), 1);
  EmitVertex();
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(-sz, -sz, -sz), 1);
  EmitVertex();
  EndPrimitive();
  
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(+sz, -sz, +sz), 1);
  EmitVertex();
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(-sz, -sz, +sz), 1);
  EmitVertex();
  EndPrimitive();
}  
