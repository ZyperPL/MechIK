#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

struct ConfigKeysValues final : public std::unordered_map<std::string, std::string>
{
  [[nodiscard]] inline float get_float(const std::string &key, const float alt = 0.0) const
  {
    if (contains(key))
      return static_cast<float>(std::stof(at(key)));
    return alt;
  }

  [[nodiscard]] inline int get_int(const std::string &key, const int alt = 0) const
  {
    if (contains(key))
      return static_cast<int>(std::stoi(at(key)));
    return alt;
  }

  [[nodiscard]] inline std::string get_string(const std::string &key, const std::string alt = "") const
  {
    if (contains(key))
      return static_cast<std::string>(at(key));
    return alt;
  }

  // returns pairs of ints extracted from comma-seperated numbers
  [[nodiscard]] inline std::pair<float, float> get_range(
    const std::string &key, const std::pair<float, float> alt = { 0.0f, 0.0f }) const
  {
    if (contains(key))
    {
      const std::string &v = at(key);
      const size_t sep = v.find(',');
      if (sep == std::string::npos || sep >= v.size() - 1)
        return alt;

      const float a = std::atof(v.substr(0, sep).data());
      const float b = std::atof(v.substr(sep + 1).data());
      return std::pair<float, float> { a, b };
    }
    return alt;
  }
};

class Config final
{
public:
  Config(const std::string file_name);
  inline std::vector<std::shared_ptr<ConfigKeysValues>> get_props_config() const { return props_config; }
  inline std::shared_ptr<ConfigKeysValues> get_world_config() const { return world_config; }

private:
  const std::string file_name;
  std::vector<std::shared_ptr<ConfigKeysValues>> props_config;
  std::shared_ptr<ConfigKeysValues> world_config;
};
