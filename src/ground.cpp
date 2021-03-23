#include "ground.hpp"

#include "ZD/ShaderLoader.hpp"

Ground::Ground()
  : ZD::Entity({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0})
{
  shader = ZD::ShaderLoader()
             .add(ZD::File("shaders/ground.vertex.glsl"), GL_VERTEX_SHADER)
             .add(ZD::File("shaders/ground.fragment.glsl"), GL_FRAGMENT_SHADER)
             .compile();

  const constexpr auto get_y = [](const float x, const float z)->float
  {
    return sin(x / 10.0) * cos(z / 2.0) * 2.0 + sin(z / 10.0) * cos(x / 12.2) * 3.1;
  };

  const constexpr auto get_n = [get_y](const float x, const float z)->glm::vec3
  {
    const glm::vec3 a{x, get_y(x, z), z};
    const glm::vec3 b{x + 1, get_y(x + 1, z), z};
    const glm::vec3 c{x, get_y(x, z + 1), z + 1};
    return glm::cross(b-a, c-a);
  };

  auto model = std::make_shared<ZD::Model>();

  for (size_t i = 0; i < 500; i++)
  for (size_t j = 0; j < 500; j++)
  {
    const float x = i - 200;
    const float z = j - 200;
    const float y0 = get_y(x, z);
    const float y1 = get_y(x, z+1);
    const float y2 = get_y(x+1, z);
    const float y3 = get_y(x+1, z+1);

    model->add_vertex(x, y0, z);
    model->add_uv(0.0, 0.0);
    glm::vec3 n = get_n(x, z);
    model->add_normal(n.x, n.y, n.z);

    model->add_vertex(x, y1, z + 1);
    model->add_uv(0.0, 1.0);
    n = get_n(x, z + 1);
    model->add_normal(n.x, n.y, n.z);

    model->add_vertex(x + 1, y2, z);
    model->add_uv(1.0, 0.0);
    n = get_n(x + 1, z);
    model->add_normal(n.x, n.y, n.z);

    model->add_vertex(x, y1, z + 1);
    model->add_uv(0.0, 1.0);
    n = get_n(x, z + 1);
    model->add_normal(n.x, n.y, n.z);

    model->add_vertex(x + 1, y2, z);
    model->add_uv(1.0, 0.0);
    n = get_n(x + 1, z);
    model->add_normal(n.x, n.y, n.z);

    model->add_vertex(x + 1, y3, z + 1);
    model->add_uv(1.0, 1.0);
    n = get_n(x + 1, z + 1);
    model->add_normal(n.x, n.y, n.z);
  }
  model->regenerate_buffers();

  auto texture = std::make_shared<ZD::Texture>(ZD::Image::load("textures/ground105_diffuse.tga"), ZD::TextureParameters { .generate_mipmap = true, .min_filter = GL_LINEAR_MIPMAP_LINEAR });
  texture->set_name("sampler");
  model->add_texture(texture);
  
  texture = std::make_shared<ZD::Texture>(ZD::Image::load("textures/ground104_diffuse.tga"), ZD::TextureParameters { .generate_mipmap = true, .min_filter = GL_LINEAR_MIPMAP_LINEAR });
  texture->set_name("sampler2");
  model->add_texture(texture);

  add_model(model);

  set_position({-250.f, 0.0f, -250.f});
}
