#pragma once

#include "ZD/3rd/glm/glm.hpp"

#include "ZD/Entity.hpp"
#include "ZD/File.hpp"
#include "ZD/Model.hpp"
#include "ZD/ModelLoader.hpp"
#include "ZD/Shader.hpp"
#include "ZD/ShaderLoader.hpp"
#include "ZD/Texture.hpp"
#include "ZD/View.hpp"

struct World;

struct LegPart : public ZD::Entity
{
  LegPart(const size_t part_index);
  virtual ~LegPart() = default;

  const size_t part_index;
  glm::quat target_rotation { 0.0f, 0.0f, 0.0f, 0.0f };
};

class Mech : public ZD::Entity
{
public:
  Mech(glm::vec3 position);
  void update(const World &world);
  void render(ZD::View &view, const World &world);

private:
  std::shared_ptr<ZD::ShaderProgram> shader;

  std::unique_ptr<ZD::Entity> body;
  std::vector<std::unique_ptr<LegPart>> legs_b;
  std::vector<std::unique_ptr<LegPart>> legs_m;
  std::vector<std::unique_ptr<LegPart>> legs_e;

  friend struct Debug;
};
