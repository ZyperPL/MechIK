#version 330 

in vec2 uv;
in float light;
uniform sampler2D sampler;

out vec4 fragColor;
void main()
{
  fragColor = texture(sampler, uv);
  fragColor.rgb += vec3(light);
}
