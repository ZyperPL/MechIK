#pragma once
#include <map>
#include <memory>
#include <random>

#include "prop.hpp"
#include "ZD/3rd/glm/glm.hpp"

class GridMap
{
public:
  struct Node
  {
    double cost { 0.0 };

    static double prop_cost(PropType &type)
    {
      switch (type)
      {
        case PropType::Bush1:
        case PropType::Bush2: return 0.4;
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
      const double normal_factor = 2.0;
      return std::min(normal_cost * normal_factor + occupied_factor, 1.0);
    }
  };

  Node &add(int i, int j) { return nodes.insert({ std::pair { i, j }, Node() }).first->second; }

  std::map<std::pair<int, int>, Node> nodes;
};
