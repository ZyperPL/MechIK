#pragma once

#include "ZD/Entity.hpp"

class Ground : public ZD::Entity
{
public:
  Ground();

  std::shared_ptr<ZD::ShaderProgram> get_shader_program() const { return shader; }

  float get_y(const float x, const float z) const;

  glm::vec3 get_n(const float x, const float z) const
  {
    const glm::vec3 a { x, get_y(x, z), z };
    const glm::vec3 b { x + 1, get_y(x + 1, z), z };
    const glm::vec3 c { x, get_y(x, z + 1), z + 1 };
    return glm::cross(b - a, c - a);
  };

  void draw(const ZD::View &view);

  void set_fog_color(const ZD::Color color)
  {
    fog_color = { color.red_float(), color.green_float(), color.blue_float() };
  }

private:
  std::shared_ptr<ZD::ShaderProgram> shader;
  const float UNIT { 10.0f };
  glm::vec3 fog_color { 0.88, 0.94, 1.0 };
};
