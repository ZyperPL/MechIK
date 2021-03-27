#include "prop.hpp"
#include "ZD/ShaderLoader.hpp"

#include "ZD/Image.hpp"

std::shared_ptr<ZD::ShaderProgram> Prop::default_shader;

Prop::Prop(const PropType type, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
: ZD::Entity(position, rotation, scale)
, type { type }
{
  switch (type)
  {
    case PropType::Tree:
    {
      auto model = std::make_shared<ZD::Model>("models/huge_tree.obj");
      const ZD::TextureParameters texture_parameters { .generate_mipmap = true,
                                                       .mag_filter = GL_NEAREST,
                                                       .min_filter = GL_LINEAR_MIPMAP_NEAREST,
                                                       .wrap_mode = GL_REPEAT };
      auto texture =
        std::make_shared<ZD::Texture>(ZD::Image::load("textures/huge_tree_diffuse.tga"), texture_parameters);
      model->add_texture(texture);

      texture =
        std::make_shared<ZD::Texture>(ZD::Image::load("textures/huge_tree_translucency.tga"), texture_parameters);
      texture->set_name("sampler_translucency");
      model->add_texture(texture);
      add_model(model);
    }
    break;

    default: assert(false); break;
  }

  if (!Prop::default_shader)
  {
    Prop::default_shader = ZD::ShaderLoader()
                             .add(ZD::File("shaders/model.vertex.glsl"), GL_VERTEX_SHADER)
                             .add(ZD::File("shaders/model.fragment.glsl"), GL_FRAGMENT_SHADER)
                             .compile();
  }
}

void Prop::draw(const ZD::View &view)
{
  auto &shader = this->shader ? *this->shader : *Prop::default_shader;
  shader.use();
  switch (type)
  {
    case PropType::Tree: shader.set_uniform<bool>("has_translucency", true); break;
    case PropType::House: break;
  }
  Entity::draw(shader, view);
}
