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

class LegEntity;

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
