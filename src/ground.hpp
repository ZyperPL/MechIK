#pragma once

#include "ZD/Entity.hpp"

class Ground : public ZD::Entity
{
public:
  Ground();

  std::shared_ptr<ZD::ShaderProgram> get_shader_program() const { return shader; }

  float get_y(const float x, const float z) const
  {
    return sin(x / 41.431) * cos(z / 1.311) * 2.13 + sin(z / 10.331) * cos(x / 12.251) * 3.511;
  };

  glm::vec3 get_n(const float x, const float z) const
  {
    const glm::vec3 a { x, get_y(x, z), z };
    const glm::vec3 b { x + 1, get_y(x + 1, z), z };
    const glm::vec3 c { x, get_y(x, z + 1), z + 1 };
    return glm::cross(b - a, c - a);
  };

private:
  std::shared_ptr<ZD::ShaderProgram> shader;
};
