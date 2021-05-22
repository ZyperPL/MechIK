#include "world.hpp"
#include "mech.hpp"

void World::generate()
{
  mech = std::make_shared<Mech>(glm::vec3 { 2.0, 8.0, 10.0 });

  static std::random_device rd;
  std::uniform_real_distribution<float> random(0.0f, 1.0f);

  grid_map = std::make_unique<GridMap>();
  std::set<std::pair<int, int>> bad_nodes;

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
      std::optional<PropType> added_prop;

      // TODO: add prop normal orientation debug option

      if (i % 3 == 0 && j % 3 == 0 && random(rd) > 0.8 && glm::distance(mech->get_position(), pos) > 5.0f)
      {
        if ((pos.y > 0.5 && pos.y < 2.0) || pos.y < -10.0)
        {
          const float theta = glm::dot(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
          const float s = sqrt((1.0f + theta) * 2.0f);
          const glm::vec3 a = glm::cross(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
          auto rot = glm::quat(s * 0.5f, a.x * (1.0f / s), a.y * (1.0f / s), a.z * (1.0f / s));
          props.push_back(Prop { PropType::Rock, pos, rot, glm::vec3 { 1.0f } });
          added_prop = PropType::Rock;
        }
        else
        {
          bool gen_bush = false;
          if (random(rd) < 0.3)
            gen_bush = true;

          if (!gen_bush)
          {
            pos -= n * 1.0f;
            n = glm::normalize(glm::vec3 { n.x, n.y * 8.0f, n.z });
            const float theta = glm::dot(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
            const float s = sqrt((1.0f + theta) * 2.0f);
            const glm::vec3 a = glm::cross(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
            auto rot = glm::quat(s * 0.5f, a.x * (1.0f / s), a.y * (1.0f / s), a.z * (1.0f / s));
            props.push_back(Prop { PropType::Tree, pos, rot, glm::vec3 { 1.0f } });
            added_prop = PropType::Tree;
            if (random(rd) < 0.7)
            {
              gen_bush = true;
              pos.x += (random(rd) - 0.5) * 10.0;
              pos.z += (random(rd) - 0.5) * 10.0;
              pos.y = ground->get_y(pos.x, pos.z);

              n = ground->get_n(pos.x, pos.z);
            }
          }

          if (gen_bush)
          {
            pos -= n * 0.2f;
            const float theta = glm::dot(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
            const float s = sqrt((1.0f + theta) * 2.0f);
            const glm::vec3 a = glm::cross(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
            auto rot = glm::quat(s * 0.5f, a.x * (1.0f / s), a.y * (1.0f / s), a.z * (1.0f / s));
            if (random(rd) < 0.7)
            {
              props.push_back(Prop { PropType::Bush2, pos, rot, glm::vec3 { 1.0f } });
              if (!added_prop)
                added_prop = PropType::Bush2;
            }
            else
            {
              props.push_back(Prop { PropType::Bush1, pos, rot, glm::vec3 { 1.0f } });
              if (!added_prop)
                added_prop = PropType::Bush1;
            }
          }
        }
      }

      map_node.cost = GridMap::Node::calculate_cost(n, added_prop);
    }
  }

  grid_map->clear_bad_nodes(*this);
}
