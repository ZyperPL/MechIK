#include "prop.hpp"

#include "ZD/Image.hpp"

Prop::Prop(const PropType type, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
: ZD::Entity(position, rotation, scale)
{
  switch (type)
  {
    case PropType::Tree:
    {
      auto model = std::make_shared<ZD::Model>("models/huge_tree.obj");
      auto texture = std::make_shared<ZD::Texture>(
        ZD::Image::load("textures/huge_tree_diffuse.tga"),
        ZD::TextureParameters { .mag_filter = GL_LINEAR, .min_filter = GL_LINEAR, .wrap_mode = GL_REPEAT });
      model->add_texture(texture);
      add_model(model);
    }
    break;

    default: assert(false); break;
  }
}
