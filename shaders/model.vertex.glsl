#version 330 

#ifdef GL_ES
  precision highp float;
#endif

uniform mat4 M; 
uniform mat4 V; 
uniform mat4 P; 

in vec3 position;
in vec2 vertex_uv;
in vec3 vertex_normal;

out vec2 uv;
out float light;

void main()
{
  gl_Position = P * V * M * vec4(position, 1.0);
  vec4 n1 = P * V * vec4(vertex_normal, 1.0);
  vec4 n2 = V * vec4(vertex_normal, 1.0);
  light = dot(n1, n2);
  uv = vertex_uv;
}
