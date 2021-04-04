#include "ground.hpp"

#include "ZD/ShaderLoader.hpp"

#define STB_PERLIN_IMPLEMENTATION
#include "3rd/stb_perlin.h"

#include "debug.hpp"

Ground::Ground()
: ZD::Entity({ 0.0, 0.0, 0.0 }, {}, { 1.0, 1.0, 1.0 })
{
  shader = ZD::ShaderLoader()
             .add(ZD::File("shaders/ground.vertex.glsl"), GL_VERTEX_SHADER)
             .add(ZD::File("shaders/ground.fragment.glsl"), GL_FRAGMENT_SHADER)
             .compile();

  auto model = ZD::Model::create();

  for (size_t i = 0; i < 200; i++)
    for (size_t j = 0; j < 200; j++)
    {
      const float x = ((float)(i)-100.0f) * UNIT;
      const float z = ((float)(j)-100.0f) * UNIT;

      const float y0 = get_y(x, z);
      const float y1 = get_y(x, z + UNIT);
      const float y2 = get_y(x + UNIT, z);
      const float y3 = get_y(x + UNIT, z + UNIT);

      model->add_vertex(x, y0, z);
      model->add_uv(0.0, 0.0);
      glm::vec3 n = get_n(x, z);
      model->add_normal(n.x, n.y, n.z);

      DBG("Ground Normals", Debug::add_line(glm::vec3 { x, y0, z }, glm::vec3 { x, y0, z } + n * 10.0f));

      model->add_vertex(x, y1, z + UNIT);
      model->add_uv(0.0, 1.0);
      n = get_n(x, z + UNIT);
      model->add_normal(n.x, n.y, n.z);

      model->add_vertex(x + UNIT, y2, z);
      model->add_uv(1.0, 0.0);
      n = get_n(x + UNIT, z);
      model->add_normal(n.x, n.y, n.z);

      model->add_vertex(x, y1, z + UNIT);
      model->add_uv(0.0, 1.0);
      n = get_n(x, z + UNIT);
      model->add_normal(n.x, n.y, n.z);

      model->add_vertex(x + UNIT, y2, z);
      model->add_uv(1.0, 0.0);
      n = get_n(x + UNIT, z);
      model->add_normal(n.x, n.y, n.z);

      model->add_vertex(x + UNIT, y3, z + UNIT);
      model->add_uv(1.0, 1.0);
      n = get_n(x + UNIT, z + UNIT);
      model->add_normal(n.x, n.y, n.z);
    }
  model->regenerate_buffers();

  const ZD::TextureParameters texture_params { .wrap = ZD::TextureWrap { UNIT / 10.0f, UNIT / 10.0f },
                                               .generate_mipmap = true,
                                               .min_filter = GL_LINEAR_MIPMAP_LINEAR,
                                               .wrap_mode = GL_REPEAT };
  auto texture = std::make_shared<ZD::Texture>(ZD::Image::load("textures/ground105_diffuse.tga"), texture_params);
  texture->set_name("sampler");
  add_texture(texture);

  texture = std::make_shared<ZD::Texture>(ZD::Image::load("textures/ground104_diffuse.tga"), texture_params);
  texture->set_name("sampler2");
  add_texture(texture);

  add_model(model);
}

void Ground::draw(const ZD::View &view)
{
  shader->use();
  shader->set_uniform<glm::vec3>("fog_color", fog_color);
  shader->set_uniform<float>("fog_scattering", 1.3);
  shader->set_uniform<float>("fog_extinction", 0.001);
  Entity::draw(*shader, view);
}

float Ground::get_y(const float x, const float z) const
{
  const float a = stb_perlin_noise3(x / 100.0f, z / 100.0f, 100.0f, 0, 0, 0) * 5.0f * UNIT;
  const float b = stb_perlin_noise3(x / 140.0f, z / 140.0f, 100.0f, 0, 0, 0) * 8.0f * UNIT;
  const float c = stb_perlin_noise3(x / 20.0f, z / 20.0f, 100.0f, 0, 0, 0) * 1.0f * UNIT;
  float d = a + b - c;
  if (d < 0.0f)
    d *= fabs(d) / (20.0f * UNIT);
  const float e = stb_perlin_noise3(x / 50.0f, z / 60.0f, 100.0f, 0, 0, 0) * 0.2f * UNIT;
  return d + e;
}
