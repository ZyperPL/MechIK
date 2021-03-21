#include "ground.hpp"

Ground::Ground()
  : ZD::Entity({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0})
{
  const constexpr auto get_y = [](const float x, const float z)->float
  {
    return sin(x / 10.0) * cos(z / 2.0) * 2.0 + sin(z / 10.0) * cos(x / 12.2) * 3.1;
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
    glm::vec3 n = glm::cross(glm::vec3{x, y0, z}, glm::vec3{x, y1, z+1.0f});
    model->add_normal(n.x, n.y, n.z);

    model->add_vertex(x, y1, z + 1);
    model->add_uv(0.0, 1.0);
    n = glm::cross(glm::vec3{x, y1, z+1.0f}, glm::vec3{x+1.0f, y2, z});
    model->add_normal(n.x, n.y, n.z);

    model->add_vertex(x + 1, y2, z);
    model->add_uv(1.0, 0.0);
    n = glm::cross(glm::vec3{x+1.0f, y2, z}, glm::vec3{x, y1, z + 1.0f});
    model->add_normal(n.x, n.y, n.z);

    model->add_vertex(x, y1, z + 1);
    model->add_uv(0.0, 1.0);
    n = glm::cross(glm::vec3{x, y1, z+1.0f}, glm::vec3{x+1.0f, y2, z});
    model->add_normal(n.x, n.y, n.z);

    model->add_vertex(x + 1, y2, z);
    model->add_uv(1.0, 0.0);
    n = glm::cross(glm::vec3{x+1.0f, y2, z}, glm::vec3{x, y1, z + 1.0f});
    model->add_normal(n.x, n.y, n.z);

    model->add_vertex(x + 1, y3, z + 1);
    model->add_uv(1.0, 1.0);
    n = glm::cross(glm::vec3{x+1.0f, y3, z + 1.0f}, glm::vec3{x + 1.0f, y2, z});
    model->add_normal(n.x, n.y, n.z);
  }
  model->regenerate_buffers();

  model->add_texture(std::make_shared<ZD::Texture>(ZD::Image::load("textures/ground105_diffuse.tga"), ZD::TextureParameters { .generate_mipmap = true, .min_filter = GL_LINEAR_MIPMAP_LINEAR } ));

  add_model(model);

  set_position({-250.f, 0.0f, -250.f});
}
