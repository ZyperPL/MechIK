#pragma once

#include "ZD/Entity.hpp"

struct Debug;
struct ConfigKeysValues;

class Ground : public ZD::Entity
{
public:
  Ground(const ConfigKeysValues &world_config);

  std::shared_ptr<ZD::ShaderProgram> get_shader_program() const { return shader; }

  float get_y(const float x, const float z) const;

  glm::vec3 get_n(const float x, const float z) const
  {
    const glm::vec3 a { x, get_y(x, z), z };
    const glm::vec3 b { x + UNIT, get_y(x + UNIT, z), z };
    const glm::vec3 c { x, get_y(x, z + UNIT), z + UNIT };
    return glm::normalize(glm::cross(c - a, b - a));
  }

  void draw(const ZD::View &view);

  void set_fog_color(const ZD::Color color)
  {
    fog_color = { color.red_float(), color.green_float(), color.blue_float() };
  }

  const float UNIT { 10.0f };

private:
  std::shared_ptr<ZD::ShaderProgram> shader;
  glm::vec3 fog_color { 0.88, 0.94, 1.0 };
  float stones_factor { 2.0f };
  float grass_factor { 32.0f };
  float stones_blur { 20.0f };
  float grass_blur { 32.0f };

  friend struct Debug;
};
