#version 330 

in vec2 uv;
in float light;
in float dst_to_camera;
in float elevation;

float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 p){
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u*u*(3.0-2.0*u);
	
	float res = mix(
		mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
		mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
	return res*res;
}

uniform sampler2D sampler;
uniform sampler2D sampler2;
uniform vec3 sky_color = vec3(0.0, 0.0, 1.0);
uniform vec2 texture_wrap;

out vec4 fragColor;
void main()
{
  vec2 nuv = uv * texture_wrap;
  float e = clamp((elevation - 0.9) * 2.0, 0.0, 1.0);
  fragColor = texture(sampler, nuv) * e + texture(sampler2, nuv) * (1.0 - e);
  fragColor += (texture(sampler2, nuv / (dst_to_camera / 2.0)) / 32.0);
  fragColor.rgb *= clamp(0.8 + light, 0.01, 1.99);
  
  float noise_value = clamp(0.1 / dst_to_camera, 0.0, 1.0);
  fragColor.rgb += noise(nuv * 12334.5232) * noise_value;

  fragColor.rgb += sky_color * clamp(dst_to_camera / 2000.0, 0.0, 1.0);
}
