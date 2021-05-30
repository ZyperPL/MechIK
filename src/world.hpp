#pragma once

#include <memory>
#include <set>
#include <unordered_map>

#include "ZD/Color.hpp"
#include "ZD/3rd/glm/glm.hpp"

#include "ground.hpp"
#include "gridmap.hpp"
#include "config.hpp"

class Mech;
class Prop;

struct World
{
  const ZD::Color sky_color { 225, 240, 255 };
  std::unique_ptr<Ground> ground;
  std::vector<std::shared_ptr<Prop>> props;
  std::unique_ptr<GridMap> grid_map;
  std::shared_ptr<Mech> mech;

  void generate(const Config &config);

  constexpr const glm::vec3 sky_color_vec() const
  {
    return { sky_color.red_float(), sky_color.green_float(), sky_color.blue_float() };
  }

  float X_SPACING { 8.0f };
  float Z_SPACING { 9.0f };
  ssize_t MIN_X { -70 };
  ssize_t MAX_X { 70 };
  ssize_t MIN_Z { -70 };
  ssize_t MAX_Z { 70 };

  friend struct Debug;
};
