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
uniform bool has_translucency = false;
uniform sampler2D sampler_translucency;

out vec4 fragColor;
void main()
{
  fragColor = texture(sampler, uv);
  if (!has_translucency && fragColor.a < 0.5)
    discard;
  
  fragColor += (texture(sampler, uv / (dst_to_camera / 2.0)) / 32.0);
  
  float tl = 0.0;
  if (has_translucency)
  {
    vec3 translucency = texture(sampler_translucency, uv).rgb;
    tl = translucency.r * translucency.g * translucency.b;
    fragColor.a -= (tl / 12.0);

    if (fragColor.a < 0.8)
      discard;

    tl = clamp(tl, 0.0, 2.0);
  }
  
  fragColor.rgb *= clamp(0.6 + light + tl, 0.0, 2.0);
  
  float noise_value = clamp(0.1 / dst_to_camera, 0.0, 1.0);
  fragColor.rgb += noise(uv * 12334.5232) * noise_value;
}
