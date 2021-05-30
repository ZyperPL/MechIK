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

    static double calculate_cost(const glm::vec3 normal, float normal_factor, std::shared_ptr<Prop> prop_occupying)
    {
      const double occupied_factor = prop_occupying ? prop_occupying->cost : 0.0;
      const double normal_cost = 1.0 - fabs(glm::dot(normal, glm::vec3 { 0.0, 1.0, 0.0 }));
      return std::min(normal_cost * normal_factor + occupied_factor, 1.0);
    }
  };
  std::map<std::pair<int, int>, Node> nodes;

  Node &add(int x, int y) { return nodes.insert({ std::pair { x, y }, Node { x, y } }).first->second; }
  std::vector<std::pair<int, int> > get_path(int start_x, int start_y, int end_x, int end_y) const;
  void clear_bad_nodes(const World &world);
};
