#version 330 

in vec2 uv;
in float light;
uniform sampler2D sampler;

out vec4 fragColor;
void main()
{
  fragColor = texture(sampler, uv);
  fragColor.rgb *= (0.6 + light);

  if (fragColor.a < 0.1)
    discard;
}
