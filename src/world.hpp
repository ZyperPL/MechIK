#pragma once

#include <memory>
#include <set>
#include <unordered_map>

#include "ZD/Color.hpp"
#include "ZD/3rd/glm/glm.hpp"

#include "prop.hpp"
#include "ground.hpp"
#include "gridmap.hpp"

class Mech;

struct World
{
  const ZD::Color sky_color { 225, 240, 255 };
  std::unique_ptr<Ground> ground;
  std::vector<Prop> props;
  std::unique_ptr<GridMap> grid_map;
  std::shared_ptr<Mech> mech;

  void generate();

  constexpr const glm::vec3 sky_color_vec() const
  {
    return { sky_color.red_float(), sky_color.green_float(), sky_color.blue_float() };
  }

  const float X_SPACING = 8.0f;
  const float Z_SPACING = 9.0f;
  const ssize_t MIN_X = -50;
  const ssize_t MAX_X = 50;
  const ssize_t MIN_Z = -50;
  const ssize_t MAX_Z = 50;

  friend struct Debug;
};
