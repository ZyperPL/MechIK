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

class LegEntity
{
public:
  LegEntity();

  void update_positions(const ZD::Entity &parent);
  void render(ZD::ShaderProgram &program, ZD::View &view);

private:
  std::vector<std::shared_ptr<ZD::Entity>> legs_b;
  std::vector<std::shared_ptr<ZD::Entity>> legs_m;
  std::vector<std::shared_ptr<ZD::Entity>> legs_e;
};

class Mech : public ZD::Entity {
public:
  Mech(glm::vec3 position);
  void render(ZD::View &view);

  static ZD::ShaderProgram *model_shader;
private:
  std::unique_ptr<ZD::Entity> body;
  std::vector<std::unique_ptr<LegEntity>> legs;
  std::shared_ptr<ZD::ShaderProgram> shader;
};
