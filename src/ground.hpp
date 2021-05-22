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
    const auto vertex_n = [&](const float x, const float z) {
      const glm::vec3 a { x, get_y(x, z), z };
      const glm::vec3 b { x + 1.0f, get_y(x + 1.0f, z), z };
      const glm::vec3 c { x, get_y(x, z + 1.0f), z + 1.0f };
      return glm::normalize(glm::cross(c - a, b - a));
    };

    const float R = 0.5f;
    glm::vec3 v { 0.0f, 0.0f, 0.0f };
    for (float zs = -R; zs <= R; zs += 0.5f)
    {
      for (float xs = -R; xs <= R; xs += 0.5f)
      {
        v += vertex_n(x, z);
      }
    }
    return glm::normalize(v);
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
