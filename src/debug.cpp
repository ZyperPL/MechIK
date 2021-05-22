#include "debug.hpp"
#include "3rd/imgui/imgui.h"
#include "mech.hpp"

std::vector<std::pair<glm::vec3, glm::vec3>> Debug::lines;
std::vector<glm::vec3> Debug::cubes;
GLuint Debug::buffer = 0;
std::shared_ptr<ZD::Model> Debug::cube;
std::shared_ptr<ZD::ShaderProgram> Debug::shader;
std::unordered_map<std::string, bool> Debug::option;

void Debug::mech_properties(Mech &mech)
{
  ImGui::Text("Position: %6.4f, %6.4f, %6.4f", mech.get_position().x, mech.get_position().y, mech.get_position().z);

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
      ImGui::Text("%6.4f, %6.4f, %6.4f, %6.4f", brot.x, brot.y, brot.z, brot.w);

      ImGui::PushID(i * 1000 + 1);
      ImGui::Text("Middle    ");
      ImGui::SameLine();
      ImGui::Text("%6.4f, %6.4f, %6.4f, %6.4f", mrot.x, mrot.y, mrot.z, mrot.w);

      ImGui::PushID(i * 1000 + 2);
      ImGui::Text("End       ");
      ImGui::SameLine();
      ImGui::Text("%6.4f, %6.4f, %6.4f, %6.4f", erot.x, erot.y, erot.z, erot.w);
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
      ImGui::Columns(3);
      ImGui::SliderAngle("X", &euler.x);
      ImGui::NextColumn();
      ImGui::SliderAngle("Y", &euler.y);
      ImGui::NextColumn();
      ImGui::SliderAngle("Z", &euler.z);
      b.set_rotation(glm::quat(euler));
      ImGui::Columns();
      
      ImGui::PushID(i * 1000 + 1);
      
      ImGui::Text("Middle   ");
      ImGui::SameLine();
      euler = glm::eulerAngles(glm::normalize(m.get_rotation()));
      ImGui::Columns(3);
      ImGui::SliderAngle("X", &euler.x);
      ImGui::NextColumn();
      ImGui::SliderAngle("Y", &euler.y);
      ImGui::NextColumn();
      ImGui::SliderAngle("Z", &euler.z);
      m.set_rotation(glm::quat(euler));
      ImGui::Columns();
      
      ImGui::PushID(i * 1000 + 2);
      
      ImGui::Text("End      ");
      ImGui::SameLine();
      euler = glm::eulerAngles(glm::normalize(e.get_rotation()));
      ImGui::Columns(3);
      ImGui::SliderAngle("X", &euler.x);
      ImGui::NextColumn();
      ImGui::SliderAngle("Y", &euler.y);
      ImGui::NextColumn();
      ImGui::SliderAngle("Z", &euler.z);
      e.set_rotation(glm::quat(euler));
      ImGui::Columns();

      ImGui::Separator();

      ImGui::PopID();
      ImGui::PopID();
      ImGui::PopID();
    }
  }
}

void Debug::mech_debug(Mech &mech)
{
  if (ImGui::Button("Show leg targets"))
  {
    for (const auto &leg_e : mech.legs_e)
    {
      add_cube(leg_e->target_position);
    }
  }
}
