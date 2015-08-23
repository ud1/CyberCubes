#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

uniform mat4 MVP;
uniform mat4 MV;
uniform vec3 eyePos;
uniform vec3 clipDir;
//uniform vec3 norm, t1, t2;
in vec4 glight[];
in vec4 slight[];
in vec2 gcolor[];
in int gtextureId[];
in int gnormalIndex[];

out vec4 fposition;
out vec4 viewSpace;
flat out vec2 fcolor;
flat out int ftextureId;
flat out mat2 fglightMat;
flat out mat2 fslightMat;
flat out int fnormalIndex;

const vec3 normals[6] = vec3[6](vec3(-1, 0, 0), vec3(1, 0, 0), vec3(0, -1, 0), vec3(0, 1, 0), vec3(0, 0, -1), vec3(0, 0, 1));
const vec3 t1s[6]     = vec3[6](vec3(0, 1, 0), vec3(0, 1, 0), vec3(1, 0, 0), vec3(1, 0, 0), vec3(1, 0, 0), vec3(1, 0, 0));
const vec3 t2s[6]     = vec3[6](vec3(0, 0, 1), vec3(0, 0, 1), vec3(0, 0, 1), vec3(0, 0, 1), vec3(0, 1, 0), vec3(0, 1, 0));

void main() {
  vec3 norm = normals[gnormalIndex[0]];
  vec3 t1 = t1s[gnormalIndex[0]];
  vec3 t2 = t2s[gnormalIndex[0]];
  
  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm - 0.5 * t1 - 0.5 * t2, glight[0].x);
  fcolor = vec2(gcolor[0].x, gcolor[0].y);
  fglightMat = mat2(glight[0].x, glight[0].y, glight[0].z, glight[0].w);
  fslightMat = mat2(slight[0].x, slight[0].y, slight[0].z, slight[0].w);
  ftextureId = gtextureId[0];
  fnormalIndex = gnormalIndex[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  viewSpace = MV * vec4(fposition.xyz, 1);
  gl_ClipDistance[0] = (fposition.x - eyePos.x) * clipDir.x;
  gl_ClipDistance[1] = (fposition.y - eyePos.y) * clipDir.y;
  gl_ClipDistance[2] = (fposition.z - eyePos.z) * clipDir.z;
  EmitVertex();

  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm + 0.5 * t1 - 0.5 * t2, glight[0].y);
  fcolor = vec2(gcolor[0].x, gcolor[0].y);
  fglightMat = mat2(glight[0].x, glight[0].y, glight[0].z, glight[0].w);
  fslightMat = mat2(slight[0].x, slight[0].y, slight[0].z, slight[0].w);
  ftextureId = gtextureId[0];
  fnormalIndex = gnormalIndex[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  viewSpace = MV * vec4(fposition.xyz, 1);
  gl_ClipDistance[0] = (fposition.x - eyePos.x) * clipDir.x;
  gl_ClipDistance[1] = (fposition.y - eyePos.y) * clipDir.y;
  gl_ClipDistance[2] = (fposition.z - eyePos.z) * clipDir.z;
  EmitVertex();
  
  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm - 0.5 * t1 + 0.5 * t2, glight[0].z);
  fcolor = vec2(gcolor[0].x, gcolor[0].y);
  fglightMat = mat2(glight[0].x, glight[0].y, glight[0].z, glight[0].w);
  fslightMat = mat2(slight[0].x, slight[0].y, slight[0].z, slight[0].w);
  ftextureId = gtextureId[0];
  fnormalIndex = gnormalIndex[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  viewSpace = MV * vec4(fposition.xyz, 1);
  gl_ClipDistance[0] = (fposition.x - eyePos.x) * clipDir.x;
  gl_ClipDistance[1] = (fposition.y - eyePos.y) * clipDir.y;
  gl_ClipDistance[2] = (fposition.z - eyePos.z) * clipDir.z;
  EmitVertex();
  
  EndPrimitive();

  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm + 0.5 * t1 - 0.5 * t2, glight[0].y);
  fcolor = vec2(gcolor[0].x, gcolor[0].y);
  fglightMat = mat2(glight[0].x, glight[0].y, glight[0].z, glight[0].w);
  fslightMat = mat2(slight[0].x, slight[0].y, slight[0].z, slight[0].w);
  ftextureId = gtextureId[0];
  fnormalIndex = gnormalIndex[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  viewSpace = MV * vec4(fposition.xyz, 1);
  gl_ClipDistance[0] = (fposition.x - eyePos.x) * clipDir.x;
  gl_ClipDistance[1] = (fposition.y - eyePos.y) * clipDir.y;
  gl_ClipDistance[2] = (fposition.z - eyePos.z) * clipDir.z;
  EmitVertex();
  
  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm - 0.5 * t1 + 0.5 * t2, glight[0].z);
  fcolor = vec2(gcolor[0].x, gcolor[0].y);
  fglightMat = mat2(glight[0].x, glight[0].y, glight[0].z, glight[0].w);
  fslightMat = mat2(slight[0].x, slight[0].y, slight[0].z, slight[0].w);
  ftextureId = gtextureId[0];
  fnormalIndex = gnormalIndex[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  viewSpace = MV * vec4(fposition.xyz, 1);
  gl_ClipDistance[0] = (fposition.x - eyePos.x) * clipDir.x;
  gl_ClipDistance[1] = (fposition.y - eyePos.y) * clipDir.y;
  gl_ClipDistance[2] = (fposition.z - eyePos.z) * clipDir.z;
  EmitVertex();
  
  fposition = vec4(gl_in[0].gl_Position.xyz + 0.5 * norm + 0.5 * t1 + 0.5 * t2, glight[0].w);
  fcolor = vec2(gcolor[0].x, gcolor[0].y);
  fglightMat = mat2(glight[0].x, glight[0].y, glight[0].z, glight[0].w);
  fslightMat = mat2(slight[0].x, slight[0].y, slight[0].z, slight[0].w);
  ftextureId = gtextureId[0];
  fnormalIndex = gnormalIndex[0];
  gl_Position = MVP * vec4(fposition.xyz, 1);
  viewSpace = MV * vec4(fposition.xyz, 1);
  gl_ClipDistance[0] = (fposition.x - eyePos.x) * clipDir.x;
  gl_ClipDistance[1] = (fposition.y - eyePos.y) * clipDir.y;
  gl_ClipDistance[2] = (fposition.z - eyePos.z) * clipDir.z;
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

