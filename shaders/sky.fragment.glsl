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

uniform vec3 sky_color = vec3(0.01, 0.01, 1.0);
uniform vec3 cloud_color = vec3(0.95, 0.98, 1.01);
uniform float time = 0.0;

out vec4 fragColor;
void main()
{
  const float ts = 641.124f;

  fragColor = vec4(sky_color, 1.0f);
  
  vec2 nuv = uv * 8.0f;
  nuv.x += time / ts;
  nuv.y += time / (ts * 0.9641f);
  float v = noise(nuv * 12.141f) * 6.741;
  v /= noise(nuv * 41.141f) * 5.741;
  v -= noise(nuv * 115.124f) * 5.214f;
  v -= noise(nuv * 315.424f) * 12.214f;
  v += noise(nuv * 22.441f) * 5.741;
  v -= noise(nuv * 1215.424f + time * 0.415) * 8.214f;
  v -= noise(nuv * 715.424f - time * 0.12) * 8.514f;

  if (v < 0.002)
    discard;

  if (v > 0.001 && v < 12.2)
    fragColor.rgb += sky_color * 2.0f;
  else
    fragColor.rgb = clamp(v, 0.1f, 1.00f) * cloud_color;

  //vec2 auv = vec2(alpha / 2. + 0.5, beta / 2. + 0.5);
}
