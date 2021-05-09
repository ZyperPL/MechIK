#include "mech.hpp"

#include "ZD/3rd/glm/gtx/rotate_vector.hpp"
#include "ZD/3rd/glm/ext/quaternion_trigonometric.hpp"

#include "world.hpp"

LegPart::LegPart(const size_t part_index)
: ZD::Entity {}
, part_index { part_index }
{
  add_texture(ZD::Texture::load(
    ZD::Image::load("textures/metal29_diffuse.tga"),
    ZD::TextureParameters { .generate_mipmap = true,
                            .mag_filter = GL_LINEAR,
                            .min_filter = GL_LINEAR_MIPMAP_LINEAR,
                            .wrap_mode = GL_REPEAT }));

  std::shared_ptr<ZD::Model> model;
  switch (part_index)
  {
    case 0: model = ZD::Model::load("models/mech_leg_b.obj"); break;
    case 1: model = ZD::Model::load("models/mech_leg_m.obj"); break;
    case 2:
    default: model = ZD::Model::load("models/mech_leg_e.obj"); break;
  }
  add_model(model);
}

Mech::Mech(glm::vec3 position)
: ZD::Entity(position, {}, { 1.0, 1.0, 1.0 })
{
  body = std::make_unique<ZD::Entity>();
  const auto body_model = ZD::Model::load("models/mech_body.obj");
  const auto metal_texture = ZD::Texture::load(
    ZD::Image::load("textures/metal29_diffuse.tga"),
    ZD::TextureParameters { .generate_mipmap = true,
                            .mag_filter = GL_LINEAR,
                            .min_filter = GL_LINEAR_MIPMAP_LINEAR,
                            .wrap_mode = GL_REPEAT });

  body->add_texture(metal_texture);
  body->add_model(body_model);
  body->set_position(position);

  const size_t LEGS_NUM = 4;
  for (size_t i = 0; i < LEGS_NUM; i++)
  {
    legs_b.push_back(std::make_unique<LegPart>(0));
    legs_m.push_back(std::make_unique<LegPart>(1));
    legs_e.push_back(std::make_unique<LegPart>(2));
  }

  shader = ZD::ShaderLoader()
             .add(ZD::File("shaders/model.vertex.glsl"), GL_VERTEX_SHADER)
             .add(ZD::File("shaders/model.fragment.glsl"), GL_FRAGMENT_SHADER)
             .compile();
}

void Mech::update([[maybe_unused]] const World &world)
{
  const float angle_step = 2.0f*M_PI / static_cast<float>(legs_b.size());

  for (size_t i = 0; i < legs_b.size(); ++i)
  {
    auto &leg_e = legs_e[i];

    const float angle = angle_step / 2.0f + i * angle_step;
    
    const glm::quat target_rotation = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
    const auto target_pos = position + target_rotation * glm::vec3(4.0f, 0.0f, 0.0f);
    leg_e->target_position.x = target_pos.x;
    leg_e->target_position.y = world.ground->get_y(target_pos.x, target_pos.z);
    leg_e->target_position.z = target_pos.z;

    leg_e->set_rotation(target_rotation);
  }

  for (size_t i = 0; i < legs_b.size(); ++i)
  {
    auto &leg_b = legs_b[i];
    const glm::vec3 b_translate = position + rotation * glm::vec3 { 0.0f, -1.0f, 0.0f };
    const glm::quat b_rotate = rotation * leg_b->get_rotation();
    leg_b->set_position(b_translate);
    leg_b->set_rotation(b_rotate);

    auto &leg_m = legs_m[i];
    const glm::vec3 m_translate = b_translate + b_rotate * glm::vec3 { 1.0f, 0.0f, 0.0f };
    const glm::quat m_rotate = b_rotate * leg_m->get_rotation();
    leg_m->set_position(m_translate);
    leg_m->set_rotation(m_rotate);

    auto &leg_e = legs_e[i];
    const glm::vec3 e_translate = m_translate + m_rotate * glm::vec3 { 1.0f, 0.0f, 0.0f };
    //const glm::quat e_rotate = m_rotate * leg_e->get_rotation();
    leg_e->set_position(e_translate);
    //leg_e->set_rotation(e_rotate);//TODO
  }
}

void Mech::render(ZD::View &view, [[maybe_unused]] const World &world)
{
  shader->use();
  body->set_position(get_position());
  body->set_rotation(get_rotation());
  body->set_scale(get_scale());
  body->draw(*shader, view);

  for (size_t i = 0; i < legs_b.size(); ++i)
  {
    legs_b[i]->draw(*shader, view);
    legs_m[i]->draw(*shader, view);
    legs_e[i]->draw(*shader, view);
  }
}
