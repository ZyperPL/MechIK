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

out vec4 position_model_space;
out vec4 position_camera_space;

void main()
{
  position_model_space = M * vec4(position, 1.0);
  position_camera_space = V * position_model_space;
  gl_Position = P * position_camera_space;
  
  light = max(dot(vertex_normal, normalize(position - gl_Position.xyz)), 0.0);
  uv = vertex_uv;
}
