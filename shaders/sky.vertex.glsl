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

out float alpha;
out float beta;

void main()
{
  vec4 position_model_space = M * vec4(position, 1.0);
  vec4 position_camera_space = V * position_model_space;
  gl_Position = P * position_camera_space;
  uv = vertex_uv;

  float dst_to_camera = length(position_camera_space);
  //if (dst_to_camera > 1000.0)
  //  gl_Position.y += dst_to_camera;

  alpha = dot(normalize(position_camera_space.xyz), normalize(position_model_space.xyz));
  beta =  dot(normalize(position_camera_space.xyz), normalize(position_model_space.xyz));
}
