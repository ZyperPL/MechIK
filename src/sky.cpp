#include "sky.hpp"

#include "ZD/Model.hpp"

Sky::Sky(const ZD::Color sky_color)
: ZD::Entity()
, sky_color { sky_color }
{
  shader = ZD::ShaderLoader()
             .add(ZD::File("shaders/sky.vertex.glsl"), GL_VERTEX_SHADER)
             .add(ZD::File("shaders/sky.fragment.glsl"), GL_FRAGMENT_SHADER)
             .compile();

  add_model(ZD::Model::load("models/sky.obj"));

  position.y = 200.0f;

  scale.x = 80.0f;
  scale.z = 80.0f;
}

void Sky::render(const ZD::View &view)
{
  static float t = 0.0;
  t += 0.1;

  shader->use();

  shader->set_uniform<glm::vec3>(
    "sky_color", { sky_color.red_float(), sky_color.green_float(), sky_color.blue_float() });
  shader->set_uniform<float>("time", t);

  position.x = view.get_position().x;
  position.y = view.get_position().y + 200.0f;
  position.z = view.get_position().z;

  Entity::render(*shader, view);
}
