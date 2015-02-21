#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

uniform mat4 MVP;
uniform vec3 norm, t1, t2;
in vec4 glight[];
in vec4 slight[];
in vec2 gcolor[];
in int gtextureId[];

out vec4 fposition;
out vec4 flight;
flat out int ftextureId;

void main() {
  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm - 0.5 * t1 - 0.5 * t2, glight[0].x);
  flight = vec4(glight[0].x, slight[0].x, gcolor[0].x, gcolor[0].y);
  ftextureId = gtextureId[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  EmitVertex();

  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm + 0.5 * t1 - 0.5 * t2, glight[0].y);
  flight = vec4(glight[0].y, slight[0].y, gcolor[0].x, gcolor[0].y);
  ftextureId = gtextureId[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  EmitVertex();
  
  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm - 0.5 * t1 + 0.5 * t2, glight[0].z);
  flight = vec4(glight[0].z, slight[0].z, gcolor[0].x, gcolor[0].y);
  ftextureId = gtextureId[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  EmitVertex();
  
  EndPrimitive();

  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm + 0.5 * t1 - 0.5 * t2, glight[0].y);
  flight = vec4(glight[0].y, slight[0].y, gcolor[0].x, gcolor[0].y);
  ftextureId = gtextureId[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  EmitVertex();
  
  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm - 0.5 * t1 + 0.5 * t2, glight[0].z);
  flight = vec4(glight[0].z, slight[0].z, gcolor[0].x, gcolor[0].y);
  ftextureId = gtextureId[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  EmitVertex();
  
  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm + 0.5 * t1 + 0.5 * t2, glight[0].w);
  flight = vec4(glight[0].w, slight[0].w, gcolor[0].x, gcolor[0].y);
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

