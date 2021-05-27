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
      const float y1 = get_y(x + UNIT, z);
      const float y2 = get_y(x + UNIT, z + UNIT);
      const float y3 = get_y(x, z + UNIT);

      model->add_vertex(x, y0, z);
      model->add_uv(0.0, 0.0);
      glm::vec3 n = get_n(x, z);
      model->add_normal(n.x, n.y, n.z);

      //DBG_ENABLE("Ground Normals");
      DBG("Ground Normals", Debug::add_line(glm::vec3 { x, y0, z }, glm::vec3 { x, y0, z } + n * 10.0f));

      model->add_vertex(x + UNIT, y1, z);
      model->add_uv(1.0, 0.0);
      n = get_n(x + UNIT, z);
      model->add_normal(n.x, n.y, n.z);

      model->add_vertex(x + UNIT, y2, z + UNIT);
      model->add_uv(1.0, 1.0);
      n = get_n(x + UNIT, z + UNIT);
      model->add_normal(n.x, n.y, n.z);

      model->add_vertex(x, y0, z);
      model->add_uv(0.0, 0.0);
      n = get_n(x, z);
      model->add_normal(n.x, n.y, n.z);

      model->add_vertex(x, y3, z + UNIT);
      model->add_uv(0.0, 1.0);
      n = get_n(x, z + UNIT);
      model->add_normal(n.x, n.y, n.z);

      model->add_vertex(x + UNIT, y2, z + UNIT);
      model->add_uv(1.0, 1.0);
      n = get_n(x + UNIT, z + UNIT);
      model->add_normal(n.x, n.y, n.z);
    }
  model->regenerate_buffers();

  const ZD::TextureParameters texture_params { .wrap = ZD::TextureWrap { UNIT / 10.0f, UNIT / 10.0f },
                                               .generate_mipmap = true,
                                               .min_filter = GL_LINEAR_MIPMAP_LINEAR,
                                               .wrap_mode = GL_REPEAT };
  auto texture = ZD::Texture::load(ZD::Image::load("textures/ground105_diffuse.tga"), texture_params);
  texture->set_name("sampler");
  add_texture(texture);

  texture = ZD::Texture::load(ZD::Image::load("textures/ground104_diffuse.tga"), texture_params);
  texture->set_name("sampler2");
  add_texture(texture);

  texture = ZD::Texture::load(ZD::Image::load("textures/ground106_diffuse.tga"), texture_params);
  texture->set_name("sampler3");
  add_texture(texture);

  add_model(model);
}

void Ground::draw(const ZD::View &view)
{
  shader->use();
  shader->set_uniform<glm::vec3>("fog_color", fog_color);
  shader->set_uniform<float>("fog_scattering", 1.25);
  shader->set_uniform<float>("fog_extinction", 0.001);
  Entity::draw(*shader, view);
}

float Ground::get_y(const float x, const float z) const
{
  const float gx = std::floor(x / UNIT) * UNIT;
  const float gz = std::floor(z / UNIT) * UNIT;

  const auto get_noise_y = [this](const float x, const float z) -> float {
    const float a = stb_perlin_noise3(x / 100.0f, z / 100.0f, 100.0f, 0, 0, 0) * 5.0f * UNIT;
    const float b = stb_perlin_noise3(x / 140.0f, z / 140.0f, 100.0f, 0, 0, 0) * 8.0f * UNIT;
    const float c = stb_perlin_noise3(x / 20.0f, z / 20.0f, 100.0f, 0, 0, 0) * 1.0f * UNIT;
    float d = a + b - c;
    if (d < 0.0f)
      d *= fabs(d) / (20.0f * UNIT);
    const float e = stb_perlin_noise3(x / 50.0f, z / 60.0f, 100.0f, 0, 0, 0) * 0.2f * UNIT;
    return d + e;
  };

  const float px = (x - gx) / UNIT;
  const float pz = (z - gz) / UNIT;

  if (px == 0.0 && pz == 0.0)
    return get_noise_y(gx, gz);

  const float y00 = get_noise_y(gx, gz);
  const float y10 = px > pz ? get_noise_y(gx + UNIT, gz) : get_noise_y(gx, gz + UNIT);
  const float y11 = get_noise_y(gx + UNIT, gz + UNIT);
  
  if ((px == 1.0 && pz == 0.0) || (px == 0.0 && pz == 1.0))
    return y10;

  const float xv1 = 0.0f;
  const float yv1 = 0.0f;
  const float xv2 = px > pz ? 1.0f : 0.0f;
  const float yv2 = px > pz ? 0.0f : 1.0f;
  const float xv3 = 1.0f;
  const float yv3 = 1.0f;

  const float b = (yv2 - yv3) * (xv1 - xv3) + (xv3 - xv2) * (yv1 - yv3);
  const float w1 = ((yv2 - yv3) * (px - xv3) + (xv3 - xv2) * (pz - yv3)) / b;
  const float w2 = ((yv3 - yv1) * (px - xv3) + (xv1 - xv3) * (pz - yv3)) / b;
  const float w3 = 1.0f - w1 - w2;

  return w1 * y00 + w2 * y10 + w3 * y11;
}
