#include "debug.hpp"
#include "3rd/imgui/imgui.h"
#include "mech.hpp"

std::vector<std::pair<glm::vec3, glm::vec3>> Debug::lines;
std::vector<std::pair<std::string, glm::vec3>> Debug::cubes;
GLuint Debug::buffer = 0;
std::shared_ptr<ZD::Model> Debug::cube;
std::shared_ptr<ZD::ShaderProgram> Debug::shader;
std::unordered_map<std::string, bool> Debug::option;
std::unordered_map<std::string, bool> Debug::enabled_cubes;

void Debug::mech_properties_position(Mech &mech)
{
  ImGui::Text("Position: %6.4f, %6.4f, %6.4f", mech.get_position().x, mech.get_position().y, mech.get_position().z);
  ImGui::DragFloat("Move Speed", &mech.move_speed, 0.1f, 0.0f, 1.0f, "%6.4f");
  ImGui::DragFloat("Height", &mech.height, 0.1f, -1.0f, 9.0f, "%6.4f");
}

void Debug::mech_properties_rotation(Mech &mech)
{
  ImGui::DragFloat("Rotation Speed", &mech.rotation_speed, 0.1f, 0.0f, 1.0f, "%6.4f");

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
  size_t legs_n = mech.get_legs_count();
  ImGui::Text("Legs count");
  ImGui::SameLine();
  if (ImGui::Button("+"))
    legs_n++;
  ImGui::SameLine();
  ImGui::Text("%lu", legs_n);
  ImGui::SameLine();
  if (ImGui::Button("-"))
    legs_n--;

  if (legs_n != mech.get_legs_count())
  {
    mech.set_legs_count(legs_n);
  }
  ImGui::Separator();

  ImGui::DragFloat("Angle Offset", &mech.angle_offset, 0.5f, 0.0f, 12.0f);

  ImGui::Separator();
}
