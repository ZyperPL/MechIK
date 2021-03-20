#version 330 

in vec2 uv;
in float light;
in float dst_to_camera;

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

out vec4 fragColor;
void main()
{
  fragColor = texture(sampler, uv);
  if (fragColor.a < 0.1)
    discard;
  
  fragColor += (texture(sampler, uv / (dst_to_camera / 2.0)) / 32.0);
  fragColor.rgb *= (0.6 + light);
  
  float noise_value = clamp(0.1 / dst_to_camera, 0.0, 1.0);
  fragColor.rgb += noise(uv * 12134.1142) * (noise_value * 2.0);
  fragColor.rgb += noise(uv * 12334.5232) * noise_value;
  fragColor.rgb += (noise(uv * 62334.1132) * (noise_value * 1.2) * fragColor.rgb);
  fragColor.rgb += noise(uv * 112334.7132) * (noise_value / 2.0);
}
