#pragma once

#include "ZD/Model.hpp"
#include "ZD/Shader.hpp"
#include "ZD/Entity.hpp"
#include "ZD/Image.hpp"

enum class PropType
{
  Tree, House
};

class Prop : public ZD::Entity
{
public:
  Prop(const PropType type, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

private:
};
