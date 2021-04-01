#version 330 

in vec2 uv;
in float alpha;
in float beta;

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

uniform vec3 sky_color = vec3(0.8, 0.9, 1.0);
uniform float time = 0.0;

out vec4 fragColor;
void main()
{
  fragColor = vec4(sky_color, 1.0f);
  
  vec2 nuv = uv * 10.0f;
  nuv.x += time / 900.0f;
  nuv.y += time / 420.0f;
  float v = clamp(noise(nuv * 12.141f) * 1.9f, 1.0f, 1.8f);

  fragColor.rgb *= v;

  vec2 auv = vec2(alpha / 2. + 0.5, beta / 2. + 0.5);
  //fragColor.rgb = pow(auv.x, 10.0f);
}
