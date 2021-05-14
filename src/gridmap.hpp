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
    Node *west { nullptr };
    Node *north { nullptr };
    Node *east { nullptr };
    Node *south { nullptr };

    double cost { 0.0 };

    static double calculate_cost(const glm::vec3 normal, std::optional<PropType> occupied)
    {
      const double occupied_factor = occupied.has_value() ? 1.0 : 0.0;
      const double normal_cost = 1.0 - fabs(glm::dot(normal, glm::vec3 { 0.0, 1.0, 0.0 }));
      const double normal_factor = 2.0;
      return std::min(normal_cost * normal_factor + occupied_factor, 1.0);
    }
  };

  Node &add(int i, int j) { return nodes.insert({ std::pair { i, j }, Node() }).first->second; }

  std::map<std::pair<int, int>, Node> nodes;
};
