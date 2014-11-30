#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

uniform mat4 MVP;
uniform vec3 norm, t1, t2;
in vec4 glight[];
in int gtextureId[];

out vec4 fposition;
flat out int ftextureId;

void main() {
  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm - 0.5 * t1 - 0.5 * t2, glight[0].x);
  ftextureId = gtextureId[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  EmitVertex();

  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm + 0.5 * t1 - 0.5 * t2, glight[0].y);
  ftextureId = gtextureId[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  EmitVertex();
  
  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm - 0.5 * t1 + 0.5 * t2, glight[0].z);
  ftextureId = gtextureId[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  EmitVertex();
  
  EndPrimitive();

  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm + 0.5 * t1 - 0.5 * t2, glight[0].y);
  ftextureId = gtextureId[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  EmitVertex();
  
  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm - 0.5 * t1 + 0.5 * t2, glight[0].z);
  ftextureId = gtextureId[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  EmitVertex();
  
  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm + 0.5 * t1 + 0.5 * t2, glight[0].w);
  ftextureId = gtextureId[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  EmitVertex();

  EndPrimitive();
} 


/*layout(points) in;
layout(triangle_strip, max_vertices = 3) out;
uniform mat4 MVP;

void main() {
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz, 1);
  //gl_Position = vec4(0.0, 0.0, 0.5, 5);
  EmitVertex();
  
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(0.5, 1.0, 0.0), 1);
  EmitVertex();
  
  gl_Position = MVP * vec4(gl_in[0].gl_Position.xyz + vec3(1.0, 0.5, 0.0), 1);
  EmitVertex();
  EndPrimitive();
} */

