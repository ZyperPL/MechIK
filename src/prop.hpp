#pragma once

#include "ZD/Model.hpp"
#include "ZD/Shader.hpp"
#include "ZD/Entity.hpp"
#include "ZD/Image.hpp"

enum class PropType
{
  Tree,
  House,
  Rock,
  Bush
};

class Prop : public ZD::Entity
{
public:
  Prop(const PropType type, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
  void draw(const ZD::View &view);

  const PropType type;
  bool has_transulency { false };
private:
  std::shared_ptr<ZD::ShaderProgram> shader;
  static std::shared_ptr<ZD::ShaderProgram> default_shader;
};
