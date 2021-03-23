#include "ground.hpp"

#include "ZD/ShaderLoader.hpp"

Ground::Ground()
: ZD::Entity({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 })
{
  shader = ZD::ShaderLoader()
             .add(ZD::File("shaders/ground.vertex.glsl"), GL_VERTEX_SHADER)
             .add(ZD::File("shaders/ground.fragment.glsl"), GL_FRAGMENT_SHADER)
             .compile();

  auto model = std::make_shared<ZD::Model>();

  const float UNIT = 1.0;

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

  auto texture = std::make_shared<ZD::Texture>(
    ZD::Image::load("textures/ground105_diffuse.tga"),
    ZD::TextureParameters { .generate_mipmap = true, .min_filter = GL_LINEAR_MIPMAP_LINEAR });
  texture->set_name("sampler");
  model->add_texture(texture);

  texture = std::make_shared<ZD::Texture>(
    ZD::Image::load("textures/ground104_diffuse.tga"),
    ZD::TextureParameters { .generate_mipmap = true, .min_filter = GL_LINEAR_MIPMAP_LINEAR });
  texture->set_name("sampler2");
  model->add_texture(texture);

  add_model(model);
}
