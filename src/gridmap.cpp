#include "gridmap.hpp"
#include "world.hpp"

#include <utility>

std::vector<std::pair<int, int>> GridMap::get_path(int start_x, int start_y, int end_x, int end_y) const
{
  const auto h_score = [&](int x, int y) -> double { return sqrt(pow(end_x - x, 2) + pow(end_y - y, 2)); };

  std::vector<std::pair<int, int>> path; // result
  std::map<std::pair<int, int>, const GridMap::Node *> open_nodes; // node candidates, neighbours of visited
  std::map<std::pair<int, int>, double> scores; // best scores from start to node
  std::map<std::pair<int, int>, double> f_scores; // prediction of best path from node to end
  std::map<std::pair<int, int>, std::pair<int, int>> next_previous; // best links between the next and the previous

  // search if start and end node does even exist
  const auto idx_node_it = nodes.find({ start_x, start_y });
  if (!nodes.contains({ end_x, end_y }) || idx_node_it == nodes.end())
    return path;

  // initial values for first (Start) node
  scores.insert({ { start_x, start_y }, 0.0 });
  f_scores.insert({ { start_x, start_y }, h_score(start_x, start_y) });
  const GridMap::Node *n = &idx_node_it->second;
  open_nodes.insert({ { start_x, start_y }, n });

  while (!open_nodes.empty())
  {
    const Node *current = open_nodes.begin()->second;
    // search for best predicted not visited node
    for (const auto &idx_node : open_nodes)
    {
      if (f_scores.at(idx_node.first) < f_scores.at({ current->x, current->y }))
        current = idx_node.second;
    }

    if (current->x == end_x && current->y == end_y)
      break;

    open_nodes.erase({ current->x, current->y });

    // check neighbours
    const double score_to_current = scores.find({ current->x, current->y })->second;
    for (int iy = -1; iy <= 1; ++iy)
      for (int ix = -1; ix <= 1; ++ix)
      {
        if (ix == 0 && iy == 0)
          continue;

        const std::pair<int, int> neighbour_idx { current->x + ix, current->y + iy };
        const auto neighbour = nodes.find(neighbour_idx);
        if (neighbour != nodes.end())
        {
          // score to neighbour from the start
          const double sc = score_to_current + neighbour->second.cost + 1.0;
          if (scores.find(neighbour_idx) == scores.end() || scores.at(neighbour_idx) > sc)
          {
            scores.insert({ neighbour_idx, sc });

            // score to neighbour and from the neighbour to the end
            const double f_sc = sc + h_score(neighbour_idx.first, neighbour_idx.second);
            f_scores.insert({ neighbour_idx, f_sc });

            next_previous.insert({ neighbour_idx, { current->x, current->y } });
            open_nodes.insert({ neighbour_idx, &neighbour->second });
          }
        }
      }
  }

  // create path from the end to the start
  std::pair<int, int> current { end_x, end_y };
  path.push_back(current);
  while (next_previous.contains(current))
  {
    current = next_previous.at(current);
    path.push_back(current);
  }

  return path;
}

void GridMap::clear_bad_nodes(const World &world)
{
  const int REMOVE_R = 1;

  for (ssize_t i = world.MIN_X; i < world.MAX_X; i++)
    for (ssize_t j = world.MIN_Z; j < world.MAX_Z; j++)
    {
      const std::pair<int, int> key { i, j };

      if (!nodes.contains(key))
        continue;

      if (nodes.at(key).cost > 0.99)
      {
        for (int iy = -REMOVE_R; iy <= REMOVE_R; ++iy)
          for (int ix = -REMOVE_R; ix <= REMOVE_R; ++ix)
            nodes.erase({ i + ix, j + iy });
      }
    }
}
