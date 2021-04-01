#pragma once

#include "ZD/Entity.hpp"
#include "ZD/ShaderLoader.hpp"

class Sky : public ZD::Entity
{
public:
  Sky(const ZD::Color sky_color);
  void render(const ZD::View &view);

private:
  std::shared_ptr<ZD::ShaderProgram> shader;
  const ZD::Color sky_color { 200, 220, 255 };
};
