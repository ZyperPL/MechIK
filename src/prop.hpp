#pragma once

#include <set>

#include "ZD/Model.hpp"
#include "ZD/Shader.hpp"
#include "ZD/Entity.hpp"
#include "ZD/Image.hpp"

struct World;

#include "config.hpp"

class Prop : public ZD::Entity
{
public:
  Prop(const std::shared_ptr<ConfigKeysValues> keys_values);
  
  void draw(const ZD::View &view, const World &world);

  bool has_transulency { false };
  const std::shared_ptr<ConfigKeysValues> keys_values;
  double cost { 1.0 };

private:
  std::shared_ptr<ZD::ShaderProgram> shader;
  static std::shared_ptr<ZD::ShaderProgram> default_shader;
};

class PropBuilder final
{
public:
  PropBuilder(const std::vector<std::shared_ptr<ConfigKeysValues>> &keys_values);
  std::shared_ptr<Prop> copy_at_position(const glm::vec3 position) const;

private:
  std::set<std::pair<std::pair<float, float>, std::shared_ptr<Prop>>> props;
};
