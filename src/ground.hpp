#pragma once

#include "ZD/Entity.hpp"

class Ground : public ZD::Entity
{
public:
  Ground();

  std::shared_ptr<ZD::ShaderProgram> get_shader_program() const { return shader; }
private:
  std::shared_ptr<ZD::ShaderProgram> shader;
};
