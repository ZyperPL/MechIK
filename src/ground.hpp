#pragma once

#include "ZD/Entity.hpp"

class Ground : public ZD::Entity
{
public:
  Ground();

  std::shared_ptr<ZD::ShaderProgram> get_shader_program() const { return shader; }

  float get_y(const float x, const float z) const
  {
    const float a = sin(x / 213.142) + cos(x / 124.421 + 321.213) * 22.312 + sin(x / 2114.14) * 141.12;
    const float b =
      sin(z / 163.142) + cos(z / 4115.1515 + 1243.142) * 1.314 + sin(z / 1241.12) * 1.1 + cos(z / 142.125) * 0.142;
    return a * b;
  };

  glm::vec3 get_n(const float x, const float z) const
  {
    const glm::vec3 a { x, get_y(x, z), z };
    const glm::vec3 b { x + 1, get_y(x + 1, z), z };
    const glm::vec3 c { x, get_y(x, z + 1), z + 1 };
    return glm::cross(b - a, c - a);
  };

  void draw(const ZD::View &view);

private:
  std::shared_ptr<ZD::ShaderProgram> shader;
};
