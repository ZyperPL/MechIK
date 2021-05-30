#include "prop.hpp"
#include "ZD/ShaderLoader.hpp"

#include "ZD/Image.hpp"

#include "world.hpp"

PropBuilder::PropBuilder(const std::vector<std::shared_ptr<ConfigKeysValues>> &keys_values)
{
  for (const auto &prop_keys_values : keys_values)
  {
    std::pair<float, float> elevation { -100000.0f, 100000.0f };
    if (prop_keys_values->contains("Elevation"))
      elevation = prop_keys_values->get_range("Elevation");

    props.insert({ elevation, std::make_shared<Prop>(prop_keys_values) });
  }
}
std::shared_ptr<Prop> PropBuilder::copy_at_position(const glm::vec3 position) const
{
  std::vector<std::shared_ptr<Prop>> valid_props;

  for (auto [elevation, prop] : props)
  {
    if (position.y >= elevation.first && position.y <= elevation.second)
    {
      valid_props.push_back(prop);
    }
  }

  if (valid_props.empty())
    return {};

  static std::random_device rd;
  std::uniform_int_distribution<int> random(0, valid_props.size() - 1);
  const size_t idx = random(rd);
  return std::make_shared<Prop>(*valid_props[idx]);
}

std::shared_ptr<ZD::ShaderProgram> Prop::default_shader;

Prop::Prop(const std::shared_ptr<ConfigKeysValues> keys_values)
: ZD::Entity(glm::vec3 { 0.0f, 0.0f, 0.0f }, glm::vec3 { 0.0f, 0.0f, 0.0f }, glm::vec3 { 1.0f, 1.0f, 1.0f })
, keys_values { keys_values }
{
  const ZD::TextureParameters texture_parameters {
    .generate_mipmap = true, .mag_filter = GL_NEAREST, .min_filter = GL_LINEAR_MIPMAP_NEAREST, .wrap_mode = GL_REPEAT
  };

  const ZD::TextureParameters translucent_texture_parameters {
    .generate_mipmap = false, .mag_filter = GL_LINEAR, .min_filter = GL_LINEAR, .wrap_mode = GL_REPEAT
  };
  auto model = ZD::Model::load(keys_values->get_string("Model"));
  auto texture = ZD::Texture::load(keys_values->get_string("TextureDiffuse"), texture_parameters);
  add_texture(texture);

  if (keys_values->contains("TextureTranslucency"))
  {
    texture = ZD::Texture::load(keys_values->get_string("TextureTranslucency"), translucent_texture_parameters);
    texture->set_name("sampler_translucency");
    add_texture(std::move(texture));
    has_transulency = true;
  }

  if (keys_values->contains("TextureNormal"))
  {
    texture = ZD::Texture::load(keys_values->get_string("TextureNormal"), translucent_texture_parameters);
    texture->set_name("sampler_normal");
    add_texture(std::move(texture));
  }

  add_model(std::move(model));

  this->scale *= keys_values->get_float("Scale", 1.0f);

  if (!Prop::default_shader)
  {
    Prop::default_shader = ZD::ShaderLoader()
                             .add(ZD::File("shaders/model.vertex.glsl"), GL_VERTEX_SHADER)
                             .add(ZD::File("shaders/model.fragment.glsl"), GL_FRAGMENT_SHADER)
                             .compile();
  }

  cost = keys_values->get_float("Cost", 1.0);

  const std::string vertex_shader_name = keys_values->get_string("VertexShader", "DEFAULT");
  const std::string fragment_shader_name = keys_values->get_string("FragmentShader", "DEFAULT");

  if (vertex_shader_name != "" && vertex_shader_name != "DEFAULT")
    if (fragment_shader_name != "" && fragment_shader_name != "DEFAULT")
    {
      auto shader_loader = ZD::ShaderLoader();
      shader_loader.add(ZD::File(vertex_shader_name), GL_VERTEX_SHADER);
      shader_loader.add(ZD::File(fragment_shader_name), GL_FRAGMENT_SHADER);
      shader = shader_loader.compile();
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
