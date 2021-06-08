#include <csignal>

#include "mech.hpp"

#include "ZD/3rd/glm/gtx/rotate_vector.hpp"
#include "ZD/3rd/glm/ext/quaternion_trigonometric.hpp"
#include "ZD/3rd/glm/gtx/quaternion.hpp"

#include "world.hpp"
#include "debug.hpp"
#include "ground.hpp"

glm::quat rotation_between_vectors(glm::vec3 start, glm::vec3 dest)
{
  start = normalize(start);
  dest = normalize(dest);

  float cosTheta = dot(start, dest);
  glm::vec3 rotationAxis;

  if (cosTheta < -1.0f + 0.001f)
  {
    rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
    if (glm::length2(rotationAxis) < 0.01f)
      rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

    rotationAxis = normalize(rotationAxis);
    return glm::angleAxis((float)M_PI, rotationAxis);
  }

  rotationAxis = cross(start, dest);

  float s = sqrt((1.0f + cosTheta) * 2.0f);
  float invs = 1.0f / s;

  return glm::quat(s * 0.5f, rotationAxis.x * invs, rotationAxis.y * invs, rotationAxis.z * invs);
}

glm::quat rotate_lookat(glm::quat q1, glm::quat q2, float max_angle)
{
  if (max_angle < 0.0001f)
    return q1;

  float cosTheta = glm::dot(q1, q2);
  if (cosTheta > 0.9999f)
    return q2;

  if (cosTheta < 0)
  {
    q1 = q1 * -1.0f;
    cosTheta *= -1.0f;
  }

  float angle = glm::acos(cosTheta);
  if (cosTheta > 0.9999f || cosTheta < -0.9999f || angle < max_angle)
    return q2;

  if (angle < 1e-4f)
    return q2;

  float fT = max_angle / angle;

  angle = max_angle;
  glm::quat res = (glm::sin((1.0f - fT) * angle) * q1 + glm::sin(fT * angle) * q2) / glm::sin(angle);
  res = glm::normalize(res);
  return res;
}

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

void LegPart::ground_collision(Ground &ground)
{
  auto e = end();
  float gye = ground.get_y(e.x, e.z);

  if (e.y < gye)
  {
    position.y += gye - e.y;
  }
  
  e = end();
  gye = ground.get_y(e.x, e.z);
  if (e.y - 0.5f < gye)
  {
    const float dst = glm::distance({ e.x, gye, e.z }, e);
    position -= rotation * forward() * (dst + 0.1f);
  }

  const float gy = ground.get_y(position.x, position.z);
  if (position.y < gy + 1.0f)
    position.y = gy + 1.0f;
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
  set_legs_count(LEGS_NUM);

  shader = ZD::ShaderLoader()
             .add(ZD::File("shaders/model.vertex.glsl"), GL_VERTEX_SHADER)
             .add(ZD::File("shaders/model.fragment.glsl"), GL_FRAGMENT_SHADER)
             .compile();
}

void Mech::set_legs_count(const size_t n)
{
  if (n <= 0 || n > 16384)
  {
    legs_b.clear();
    legs_m.clear();
    legs_e.clear();
    return;
  }

  legs_b.resize(n);
  legs_m.resize(n);
  legs_e.resize(n);

  for (size_t i = 0; i < n; i++)
  {
    legs_b[i] = std::make_unique<LegPart>(0);
    legs_m[i] = std::make_unique<LegPart>(1);
    legs_e[i] = std::make_unique<LegPart>(2);
  }
}

void Mech::step_path(const World &world)
{
  if (path.size() < 2)
    return;

  const auto get_3d_position = [&world](const auto &xz)
  {
    const float pt_x = xz.first * world.X_SPACING;
    const float pt_z = xz.second * world.Z_SPACING;
    return glm::vec3 { pt_x, world.ground->get_y(pt_x, pt_z), pt_z };
  };

  glm::vec3 closest_path_point = get_3d_position(path.front());
  glm::vec3 next_path_point = get_3d_position(*(path.begin() + 1));

  for (size_t i = 0; i < path.size(); ++i)
  {
    const auto &coord = path[i];
    const glm::vec3 path_point = get_3d_position(coord);
    if (glm::distance(position, path_point) < glm::distance(position, closest_path_point))
    {
      closest_path_point = path_point;
      if (i < path.size() - 1)
        next_path_point = get_3d_position(path[i + 1]);
    }
  }

  if (closest_path_point == next_path_point && glm::distance(next_path_point, position) < 4.0f)
    return;

  auto move_dir = glm::normalize(next_path_point - position);
  move_vec = move_dir * move_speed;
  if (glm::length(move_vec) > 10000.0f || glm::isnan(move_vec.x))
    return; 
  position += move_vec;

  const glm::vec3 FORWARD { 0.0f, 0.0f, 1.0f };
  move_dir.y = 0.0f;
  if (glm::length(move_vec) < 0.001f)
    return;
  rotation = rotate_lookat(rotation, rotation_between_vectors(FORWARD, glm::normalize(move_dir)), rotation_speed);
}

void Mech::update([[maybe_unused]] const World &world)
{
  // move and rotate based on path
  step_path(world);

  position.y = world.ground->get_y(position.x, position.z) + height;

  if (legs_b.empty())
    return;

  // animate legs
  calculate_legs(world);

  const glm::vec3 normal = world.ground->get_n(position.x, position.z);
  Debug::clear_lines("Mech Normal");
  Debug::add_line("Mech Normal", position, position + normal * 30.0f);
  auto rot_y = rotation_between_vectors(glm::vec3 { 0.0f, 1.0f, 0.0f }, glm::normalize(normal));
  body->set_position(position + glm::vec3 { 0.0f, 0.0f, 0.0f });
  body->set_rotation(rot_y * rotation);
}

void Mech::calculate_legs([[maybe_unused]] const World &world)
{
  const float angle_step = 2.0f * M_PI / static_cast<float>(legs_b.size());
  const auto leg_angle = [&angle_step, this](auto leg_n) -> float
  { return static_cast<float>(leg_n) * angle_step + (angle_step / 2.0f) * angle_offset; };

  Debug::clear_cubes("Legs Target");
  Debug::clear_cubes("Legs Current Target");

  if (legs_e.size() <= 0)
    return;

  const auto normal = world.ground->get_n(position.x, position.z);
  const glm::vec3 ground { position.x, world.ground->get_y(position.x, position.z), position.z };
  const float L_LENGTH = legs_e[0]->length() + legs_m[0]->length() + legs_b[0]->length();

  // set targets
  for (size_t i = 0; i < legs_e.size(); ++i)
  {
    auto &le = legs_e[i];
    [[maybe_unused]] auto &lm = legs_e[i];
    [[maybe_unused]] auto &lb = legs_e[i];

    const float angle = leg_angle(i);
    const glm::quat rot = rotation * glm::angleAxis(angle, normal);
    const auto leg_forward = rotation * (glm::vec3 { 0.0f, 0.0f, 1.0f + legs_next_step_distance } + move_vec);
    const auto leg_spacing_vec = rot * glm::vec3 { legs_spacing, 0.0f, 0.0f };
    auto target = position + leg_forward + leg_spacing_vec;

    target.y = world.ground->get_y(target.x, target.z);

    const auto ground_dir = glm::normalize(ground - target);
    if (glm::isnan(ground_dir).x)
      continue;

    const auto range_dst = glm::distance(position, target) - L_LENGTH;
    if (range_dst > 1000.0)
      continue;

    if (range_dst > 0.0f)
    {
      target += ground_dir * (0.7f + glm::abs(range_dst));
      target.y = world.ground->get_y(target.x, target.z);
    }

    if (glm::distance(position, le->target_position) > L_LENGTH)
    {
      le->target_position = target;
    }
    if (glm::distance(le->end(), target) >= legs_max_distance)
    {
      le->target_position = target;
    }

    Debug::add_cube("Legs Target", target);
    Debug::add_cube("Legs Current Target", le->target_position);
  }

  const float RSPEED = this->legs_rotation_speed / static_cast<float>(this->ik_iterations);
  glm::vec3 leg_center { 0.0f, 0.0f, 0.0f };
  // inverse kinematics
  for (size_t i = 0; i < ik_iterations; ++i)
  {
    for (size_t l = 0; l < legs_e.size(); ++l)
    {
      auto &le = legs_e[l];
      auto &lm = legs_m[l];
      auto &lb = legs_b[l];

      // inverse
      if (glm::distance(le->target_position, le->begin()) > 1e-2)
      {
        // TODO: refactor into single method
        auto le_dir = glm::normalize(le->target_position - le->begin());
        le_dir = glm::normalize(le_dir * le->rotation_scalar);
        if (!glm::isnan(le_dir).x)
        {
          le->set_rotation(rotate_lookat(le->get_rotation(), rotation_between_vectors(le->forward(), le_dir), RSPEED));
          le->set_position(le->target_position);
          le->ground_collision(*world.ground);
        }
      }

      if (glm::distance(le->begin(), lm->begin()) > 1e-2)
      {
        auto lm_dir = glm::normalize(le->begin() - lm->begin());
        lm_dir = glm::normalize(lm_dir * lm->rotation_scalar);
        if (!glm::isnan(lm_dir).x)
        {
          lm->set_rotation(rotate_lookat(lm->get_rotation(), rotation_between_vectors(le->forward(), lm_dir), RSPEED));
          lm->set_position(le->begin() - lm_dir * lm->length());
          lm->ground_collision(*world.ground);
        }
      }

      if (glm::distance(lm->begin(), lb->begin()) > 1e-2)
      {
        auto lb_dir = glm::normalize(lm->begin() - lb->begin());
        lb_dir = glm::normalize(lb_dir * lb->rotation_scalar);
        if (!glm::isnan(lb_dir).x)
        {
          lb->set_rotation(rotate_lookat(lb->get_rotation(), rotation_between_vectors(le->forward(), lb_dir), RSPEED));
          lb->set_position(lm->begin() - lb_dir * lb->length());
          lb->ground_collision(*world.ground);
        }
      }

      // forward
      lb->set_position(position);
      lm->set_position(lb->end());
      le->set_position(lm->end());

      if (i == ik_iterations - 1)
      {
        leg_center += le->end();
      }
    }
  }

  //position += (leg_center - position) / static_cast<float>(legs_e.size()) * 0.0001f;
  position.y += (position.y - (leg_center.y / static_cast<float>(legs_e.size()) + height)) * 0.4f;
}

void Mech::draw(ZD::View &view, [[maybe_unused]] const World &world)
{
  shader->use();
  body->render(*shader, view);

  for (size_t i = 0; i < legs_b.size(); ++i)
  {
    legs_b[i]->render(*shader, view);
    legs_m[i]->render(*shader, view);
    legs_e[i]->render(*shader, view);
  }
}
