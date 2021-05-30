#include "world.hpp"
#include "mech.hpp"
#include "prop.hpp"

#include "config.hpp"

void World::generate(const Config &config)
{
  mech = std::make_shared<Mech>(glm::vec3 { 2.0, 8.0, 10.0 });

  static std::random_device rd;
  std::uniform_real_distribution<float> random(0.0f, 1.0f);

  grid_map = std::make_unique<GridMap>();
  std::set<std::pair<int, int>> bad_nodes;

  PropBuilder prop_builder(config.get_props_config());

  const int PROP_X_SPACING = config.get_world_config()->get_int("PropSpacingX", 2);
  const int PROP_Z_SPACING = config.get_world_config()->get_int("PropSpacingZ", 4);
  const float PROP_PROBABILITY = config.get_world_config()->get_float("PropProbability", 0.9);
  MIN_X = config.get_world_config()->get_int("MinX", -200);
  MAX_X = config.get_world_config()->get_int("MaxX", 200);
  MIN_Z = config.get_world_config()->get_int("MinZ", -200);
  MAX_Z = config.get_world_config()->get_int("MaxZ", 200);
  X_SPACING = config.get_world_config()->get_float("XSpacing", X_SPACING);
  Z_SPACING = config.get_world_config()->get_float("ZSpacing", Z_SPACING);

  const float normal_factor = config.get_world_config()->get_float("NormalCostFactor", 3.0f);

  for (ssize_t i = MIN_X; i < MAX_X; i++)
  {
    for (ssize_t j = MIN_Z; j < MAX_Z; j++)
    {
      glm::vec3 pos { 0.0, -2.0, 0.0 };
      pos.x += i * X_SPACING + (random(rd) - 0.5) * X_SPACING / 2.0f;
      pos.z += j * Z_SPACING + (random(rd) - 0.5) * Z_SPACING / 2.0f;
      pos.y = ground->get_y(pos.x, pos.z);

      auto &map_node = grid_map->add(static_cast<int>(i), static_cast<int>(j));

      auto n = ground->get_n(pos.x, pos.z);
      std::shared_ptr<Prop> added_prop;

      if (
        i % PROP_X_SPACING == 0 && j % PROP_Z_SPACING == 0 && random(rd) > (1.0 - PROP_PROBABILITY) &&
        glm::distance(mech->get_position(), pos) > 5.0f)
      {
        added_prop = prop_builder.copy_at_position(pos);
        if (!added_prop)
        {
          fprintf(stderr, "Cannot create prop at position %f,%f,%f\n", pos.x, pos.y, pos.z);
          continue;
        }
        n = glm::normalize(glm::vec3 { n.x, n.y + added_prop->keys_values->get_float("OrientationY", 0.0f), n.z });
        const float theta = glm::dot(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
        const float s = sqrt((1.0f + theta) * 2.0f);
        const glm::vec3 a = glm::cross(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
        auto rot = glm::quat(s * 0.5f, a.x * (1.0f / s), a.y * (1.0f / s), a.z * (1.0f / s));
        added_prop->set_position(pos);
        added_prop->set_rotation(rot);
        props.push_back(added_prop);
      }

      map_node.cost = GridMap::Node::calculate_cost(n, normal_factor, added_prop);
    }
  }

  grid_map->clear_bad_nodes(*this);
}
