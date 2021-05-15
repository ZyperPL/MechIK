#include "prop.hpp"
#include "ZD/ShaderLoader.hpp"

#include "ZD/Image.hpp"

#include "world.hpp"

std::shared_ptr<ZD::ShaderProgram> Prop::default_shader;

Prop::Prop(const PropType type, glm::vec3 position, glm::quat rotation, glm::vec3 scale)
: ZD::Entity(position, rotation, scale)
, type { type }
{
  const ZD::TextureParameters texture_parameters {
    .generate_mipmap = true, .mag_filter = GL_NEAREST, .min_filter = GL_LINEAR_MIPMAP_NEAREST, .wrap_mode = GL_REPEAT
  };

  const ZD::TextureParameters translucent_texture_parameters {
    .generate_mipmap = false, .mag_filter = GL_LINEAR, .min_filter = GL_LINEAR, .wrap_mode = GL_REPEAT
  };
  switch (type)
  {
    case PropType::Tree:
    {
      auto model = ZD::Model::load("models/huge_tree.obj");

      auto texture = ZD::Texture::load("textures/huge_tree_diffuse.tga", texture_parameters);
      add_texture(texture);

      texture = ZD::Texture::load("textures/huge_tree_translucency.tga", translucent_texture_parameters);
      texture->set_name("sampler_translucency");
      add_texture(std::move(texture));
      add_model(std::move(model));
      has_transulency = true;
    }
    break;
    case PropType::Rock:
    {
      auto model = ZD::Model::load("models/Rock2_LOD_8k.obj");
      auto texture = ZD::Texture::load("textures/Rock2_LOD_8k_diffuse.tga", texture_parameters);
      add_texture(std::move(texture));

      texture = ZD::Texture::load("textures/Rock2_LOD_8k_normals.tga", texture_parameters);
      texture->set_name("sampler_normal");
      add_texture(std::move(texture));
      add_model(std::move(model));
      this->scale *= 0.5f + fmodf((float)(rand() % 130), 2.0f);
    }
    break;
    case PropType::Bush1:
    {
      auto model = ZD::Model::load("models/bush_05.obj");

      auto texture = ZD::Texture::load("textures/bush_05_diffuse.tga", texture_parameters);
      add_texture(texture);

      texture = ZD::Texture::load("textures/bush_05_translucency.tga", translucent_texture_parameters);
      texture->set_name("sampler_translucency");
      add_texture(std::move(texture));
      add_model(std::move(model));
      has_transulency = true;
    }
    break;
    case PropType::Bush2:
    {
      auto model = ZD::Model::load("models/bush_03.obj");

      auto texture = ZD::Texture::load("textures/bush_03_diffuse.tga", texture_parameters);
      add_texture(texture);

      texture = ZD::Texture::load("textures/bush_03_translucency.tga", translucent_texture_parameters);
      texture->set_name("sampler_translucency");
      add_texture(std::move(texture));
      add_model(std::move(model));
      has_transulency = true;
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

void Prop::draw(const ZD::View &view, const World &world)
{
  const float distance_to_camera = glm::distance(this->position, view.get_position());
  if (distance_to_camera > 1000.0f)
    return;

  if (has_transulency && distance_to_camera > 800.0f)
    return;

  auto &shader = this->shader ? *this->shader : *Prop::default_shader;
  shader.use();

  if (has_transulency && distance_to_camera < 400.0f)
    shader.set_uniform<bool>("has_translucency", true);
  else
    shader.set_uniform<bool>("has_translucency", false);

  shader.set_uniform<glm::vec3>("fog_color", world.sky_color_vec());
  shader.set_uniform<float>("fog_scattering", 1.3f);
  shader.set_uniform<float>("fog_extinction", 0.001f);

  Entity::draw(shader, view);
}
