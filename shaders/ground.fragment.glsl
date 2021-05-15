#version 330

in vec2 uv;
in vec3 normal;
in vec4 position_model_space;
in vec4 position_camera_space;

float rand(vec2 n) { return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453); }

float noise(vec2 p)
{
  vec2 ip = floor(p);
  vec2 u = fract(p);
  u = u * u * (3.0 - 2.0 * u);

  float res = mix(
    mix(rand(ip), rand(ip + vec2(1.0, 0.0)), u.x), mix(rand(ip + vec2(0.0, 1.0)), rand(ip + vec2(1.0, 1.0)), u.x), u.y);
  return res * res;
}

uniform sampler2D sampler;
uniform sampler2D sampler2;
uniform sampler2D sampler3;
uniform vec3 fog_color = vec3(0.0, 0.0, 1.0);
uniform float fog_scattering = 1.0;
uniform float fog_extinction = 0.0001;
uniform vec2 texture_wrap;

out vec4 fragColor;

void main()
{
  vec2 nuv = uv * texture_wrap;
  float dst = length(position_camera_space);
  
  float e = clamp((32.0 + position_model_space.y) / 32.0, 0.0, 1.0);

  vec4 t1 = texture(sampler, nuv);
  vec4 t2 = texture(sampler2, nuv) * e + (1.0 - e) * texture(sampler3, nuv);

  e = clamp((position_model_space.y + 2.0) / 20.0, 0.0, 1.0);
  fragColor = t1 * e + t2 * (1.0 - e);
  
  float t = clamp(dot(normal, vec3(0.2, 1.0, -0.3)), 0.0, 1.0);
  vec3 light = vec3(1.31, 1.32, 1.34) * t;
  fragColor.rgb *= light;

  if (dst < 100.0)
  {
    float noise_value = clamp(0.1 / dst, 0.0, 1.0);
    fragColor.rgb += noise(nuv * 12334.5232) * noise_value;
  }

  float fog_alpha = fog_scattering * exp(-position_model_space.y * fog_extinction) *
                    (1.0f - exp(-dst * normalize(position_camera_space).y * fog_extinction)) /
                    normalize(position_camera_space).y;

  fragColor.rgb = mix(fragColor.rgb, fog_color, clamp(fog_alpha, 0.0, 0.99));
}
