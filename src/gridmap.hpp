#pragma once
#include <map>
#include <memory>
#include <random>

#include "prop.hpp"
#include "ZD/3rd/glm/glm.hpp"

struct World;

class GridMap
{
public:
  struct Node
  {
    const int x;
    const int y;
    double cost { 0.0 };

    static double prop_cost(PropType &type)
    {
      switch (type)
      {
        case PropType::Bush1:
        case PropType::Bush2: return 0.1;
        case PropType::House: return 1.0;
        case PropType::Rock: return 1.0;
        case PropType::Tree: return 1.0;
      }
      return 1.0;
    }

    static double calculate_cost(const glm::vec3 normal, std::optional<PropType> occupied)
    {
      const double occupied_factor = occupied.has_value() ? prop_cost(occupied.value()) : 0.0;
      const double normal_cost = 1.0 - fabs(glm::dot(normal, glm::vec3 { 0.0, 1.0, 0.0 }));
      const double normal_factor = 2.7;
      return std::min(normal_cost * normal_factor + occupied_factor, 1.0);
    }
  };
  std::map<std::pair<int, int>, Node> nodes;

  Node &add(int x, int y) { return nodes.insert({ std::pair { x, y }, Node { x, y } }).first->second; }
  std::vector<std::pair<int, int> > get_path(int start_x, int start_y, int end_x, int end_y) const;
  void clear_bad_nodes(const World &world);
};
