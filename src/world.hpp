#pragma once

#include "ZD/Color.hpp"
#include <memory>
#include "prop.hpp"
#include "ground.hpp"
#include "ZD/3rd/glm/glm.hpp"

struct World
{
  const ZD::Color sky_color { 225, 240, 255 };
  std::unique_ptr<Ground> ground;
  std::vector<Prop> props;

  constexpr const glm::vec3 sky_color_vec() const
  {
    return { sky_color.red_float(), sky_color.green_float(), sky_color.blue_float() };
  }
};
