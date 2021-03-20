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
out float dst_to_camera;

void main()
{
  vec4 position_camera_space = V * M * vec4(position, 1.0);
  gl_Position = P * V * M * vec4(position, 1.0);
  light = max(dot(vertex_normal, normalize(position - gl_Position.xyz)), 0.0);
  uv = vertex_uv;
  dst_to_camera = length(position_camera_space);
}
