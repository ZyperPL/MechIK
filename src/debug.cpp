#include "debug.hpp"
#include "3rd/imgui/imgui.h"

#include "mech.hpp"
#include "ground.hpp"

std::vector<std::pair<std::string, std::pair<glm::vec3, glm::vec3>>> Debug::lines;
std::vector<std::pair<std::string, glm::vec3>> Debug::cubes;
GLuint Debug::buffer = 0;
std::shared_ptr<ZD::Model> Debug::cube;
std::shared_ptr<ZD::ShaderProgram> Debug::shader;
std::unordered_map<std::string, bool> Debug::option;
std::unordered_map<std::string, bool> Debug::enabled_cubes;

void float_drag_buttons(
  const std::string &name, float &value, float step = 0.1f, float min_v = 0.0f, float max_v = 1.0f)
{
  ImGui::PushID(name.c_str());
  ImGui::PushID(&value);
  ImGui::DragFloat(name.c_str(), &value, step, min_v, max_v, "%6.4f");
  ImGui::SameLine();
  if (ImGui::Button("-"))
    value -= step;
  ImGui::SameLine();
  if (ImGui::Button("+"))
    value += step;
  ImGui::PopID();
  ImGui::PopID();
  ImGui::Separator();
}

void Debug::mech_properties_position(Mech &mech)
{
  if (ImGui::Button("Reset Position"))
  {
    mech.position = glm::vec3(0.0f, 0.0f, 0.0f);
    mech.body->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    const auto legs_count = mech.get_legs_count();
    mech.set_legs_count(0);
    mech.set_legs_count(legs_count);
    mech.set_rotation(glm::vec3(0.0f, 0.0f, 0.0f));
    mech.set_scale(glm::vec3(1.0f));
  }

  ImGui::Text("Position: %6.4f, %6.4f, %6.4f", mech.get_position().x, mech.get_position().y, mech.get_position().z);

  ImGui::Separator();
  float_drag_buttons("Move Speed", mech.move_speed, 0.01f, 0.0f, 1.0f);
  float_drag_buttons("Height", mech.height, 0.05f, -1.0f, 8.0f);
  ImGui::Separator();
}

void Debug::mech_properties_rotation(Mech &mech)
{
  float_drag_buttons("Rotation Speed", mech.rotation_speed, 0.05f, 0.0f, 1.0f);
  ImGui::Separator();

  static bool as_quaternions = true;
  ImGui::Checkbox("As Quaternions", &as_quaternions);

  if (as_quaternions)
  {
    ImGui::Text(
      "Rotation: %6.4f, %6.4f, %6.4f, %6.4f",
      mech.get_rotation().x,
      mech.get_rotation().y,
      mech.get_rotation().z,
      mech.get_rotation().w);
    // legs info
    for (size_t i = 0; i < mech.legs_b.size(); i++)
    {
      ImGui::PushID(i * 1000 + 0);
      LegPart &b = *mech.legs_b[i];
      const auto brot = b.get_rotation();
      LegPart &m = *mech.legs_m[i];
      const auto mrot = m.get_rotation();
      LegPart &e = *mech.legs_e[i];
      const auto erot = e.get_rotation();

      ImGui::Text("Leg %lu.", i);
      ImGui::Text("Beginning ");
      ImGui::SameLine();
      ImGui::Text("% -6.4f, % -6.4f, % -6.4f, % -6.4f", brot.x, brot.y, brot.z, brot.w);

      ImGui::PushID(i * 1000 + 1);
      ImGui::Text("Middle    ");
      ImGui::SameLine();
      ImGui::Text("% -6.4f, % -6.4f, % -6.4f, % -6.4f", mrot.x, mrot.y, mrot.z, mrot.w);

      ImGui::PushID(i * 1000 + 2);
      ImGui::Text("End       ");
      ImGui::SameLine();
      ImGui::Text("% -6.4f, % -6.4f, % -6.4f, % -6.4f", erot.x, erot.y, erot.z, erot.w);
      ImGui::Separator();

      ImGui::PopID();
      ImGui::PopID();
      ImGui::PopID();
    }
  }
  else
  {
    glm::vec3 euler = glm::eulerAngles(glm::normalize(mech.get_rotation()));
    ImGui::Columns(3);
    ImGui::SliderAngle("X", &euler.x);
    ImGui::NextColumn();
    ImGui::SliderAngle("Y", &euler.y);
    ImGui::NextColumn();
    ImGui::SliderAngle("Z", &euler.z);
    mech.set_rotation(euler);
    ImGui::Columns();

    for (size_t i = 0; i < mech.legs_b.size(); i++)
    {
      ImGui::PushID(i * 1000 + 0);

      LegPart &b = *mech.legs_b[i];
      LegPart &m = *mech.legs_m[i];
      LegPart &e = *mech.legs_e[i];

      ImGui::Text("Leg %lu.", i);

      ImGui::Text("Beginning ");
      ImGui::SameLine();
      euler = glm::eulerAngles(glm::normalize(b.get_rotation()));
      ImGui::Text("% -6.4f, % -6.4f, % -6.4f", euler.x, euler.y, euler.z);

      ImGui::PushID(i * 1000 + 1);

      ImGui::Text("Middle   ");
      ImGui::SameLine();
      euler = glm::eulerAngles(glm::normalize(m.get_rotation()));
      ImGui::Text("% -6.4f, % -6.4f, % -6.4f", euler.x, euler.y, euler.z);

      ImGui::PushID(i * 1000 + 2);

      ImGui::Text("End      ");
      ImGui::SameLine();
      euler = glm::eulerAngles(glm::normalize(e.get_rotation()));
      ImGui::Text("% -6.4f, % -6.4f, % -6.4f", euler.x, euler.y, euler.z);

      ImGui::Separator();

      ImGui::PopID();
      ImGui::PopID();
      ImGui::PopID();
    }
  }
}

void Debug::mech_properties_legs(Mech &mech)
{
  ImGui::PushID("LegsCount");

  size_t legs_n = mech.get_legs_count();
  ImGui::Text("Legs count");
  ImGui::SameLine();
  if (ImGui::Button("-"))
    legs_n--;
  ImGui::SameLine();
  ImGui::Text("%lu", legs_n);
  ImGui::SameLine();
  if (ImGui::Button("+"))
    legs_n++;

  if (legs_n != mech.get_legs_count())
  {
    mech.set_legs_count(legs_n);
  }
  ImGui::PopID();

  ImGui::Separator();
  float_drag_buttons("Angle Offset", mech.angle_offset, 0.5f, 0.0f, 6.0f);
  ImGui::Separator();

  float_drag_buttons("Legs Spacing", mech.legs_spacing, 0.1f, 0.0f, 9.0f);
  ImGui::Separator();

  float_drag_buttons("Max Distance", mech.legs_max_distance, 0.1f, 0.0f, 10.0f);
  ImGui::Separator();

  float_drag_buttons("Next Step Distance", mech.legs_next_step_distance, 0.1f, 0.0f, 10.0f);
  ImGui::Separator();

  ImGui::PushID("LegsIKIterations");
  static int ik_iterations = mech.ik_iterations;
  ImGui::DragInt("IK Iterations", &ik_iterations, 1, 1, 200);
  ImGui::SameLine();
  if (ImGui::Button("-") && ik_iterations > 0)
    ik_iterations -= 0;
  ImGui::SameLine();
  if (ImGui::Button("+"))
    ik_iterations += 1;

  if (ik_iterations < 0)
    ik_iterations = 1;
  mech.ik_iterations = static_cast<size_t>(ik_iterations);
  ImGui::PopID();

  ImGui::Separator();
  float_drag_buttons("Rotation Speed", mech.legs_rotation_speed, 0.001f, 0.0f, 1.0f);
  ImGui::Separator();

  auto rotation_scalar_checkbox = [](auto &leg) {
    bool lock_x = leg.rotation_scalar.x == 0.0f;
    bool lock_y = leg.rotation_scalar.y == 0.0f;
    bool lock_z = leg.rotation_scalar.z == 0.0f;
    ImGui::Checkbox("X", &lock_x);
    ImGui::SameLine();
    ImGui::Checkbox("Y", &lock_y);
    ImGui::SameLine();
    ImGui::Checkbox("Z", &lock_z);
    leg.rotation_scalar.x = lock_x ? 0.0f : 1.0f;
    leg.rotation_scalar.y = lock_y ? 0.0f : 1.0f;
    leg.rotation_scalar.z = lock_z ? 0.0f : 1.0f;
  };
  ImGui::PushID("LegsLockAxis");
  ImGui::Text("Lock Rotation Direction:");
  static bool individually = true;
  ImGui::Checkbox("Individually", &individually);

  if (individually)
  {
    for (size_t i = 0; i < mech.legs_e.size(); ++i)
    {
      ImGui::Text("Leg %lu", i);
      ImGui::PushID(i * 1200 + 0);
      ImGui::Text("Segment 1/3");
      ImGui::SameLine();
      rotation_scalar_checkbox(*mech.legs_b[i]);
      ImGui::PopID();
      ImGui::PushID(i * 1200 + 1);
      ImGui::Text("Segment 2/3");
      ImGui::SameLine();
      rotation_scalar_checkbox(*mech.legs_m[i]);
      ImGui::PopID();
      ImGui::PushID(i * 1200 + 2);
      ImGui::Text("Segment 3/3");
      ImGui::SameLine();
      rotation_scalar_checkbox(*mech.legs_e[i]);
      ImGui::PopID();
    }
  }
  else
  {
    ImGui::PushID(1300 + 0);
    ImGui::Text("Segment 1/3");
    ImGui::SameLine();
    rotation_scalar_checkbox(*mech.legs_b[0]);
    ImGui::PopID();
    ImGui::PushID(1300 + 1);
    ImGui::Text("Segment 2/3");
    ImGui::SameLine();
    rotation_scalar_checkbox(*mech.legs_m[0]);
    ImGui::PopID();
    ImGui::PushID(1300 + 2);
    ImGui::Text("Segment 3/3");
    ImGui::SameLine();
    rotation_scalar_checkbox(*mech.legs_e[0]);
    ImGui::PopID();

    for (size_t i = 1; i < mech.legs_e.size(); ++i)
    {
      mech.legs_b[i]->rotation_scalar = mech.legs_b[0]->rotation_scalar;
      mech.legs_m[i]->rotation_scalar = mech.legs_m[0]->rotation_scalar;
      mech.legs_e[i]->rotation_scalar = mech.legs_e[0]->rotation_scalar;
    }
  }
  ImGui::PopID();
  ImGui::Separator();
}

void Debug::ground_properties(Ground &ground)
{
  ImGui::Text("UNIT = %10.5f", ground.UNIT);
  ImGui::Separator();
  ImGui::DragFloat("Stones Factor", &ground.stones_factor, 0.1, -100.0f, 100.0f);
  ImGui::DragFloat("Grass Factor", &ground.grass_factor, 0.1, -100.0f, 100.0f);
  
  ImGui::Separator();
  
  ImGui::DragFloat("Stones Blur", &ground.stones_blur, 0.1, -100.0f, 100.0f);
  ImGui::DragFloat("Grass  Blur", &ground.grass_blur, 0.1, -100.0f, 100.0f);
}
