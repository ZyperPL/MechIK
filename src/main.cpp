#include <memory>

#include "ZD/Entity.hpp"
#include "ZD/Input.hpp"
#include "ZD/Model.hpp"
#include "ZD/ModelLoader.hpp"
#include "ZD/OpenGLRenderer.hpp"
#include "ZD/ShaderLoader.hpp"
#include "ZD/Window.hpp"

#include "mech.hpp"
#include "prop.hpp"
#include "ground.hpp"
#include "sky.hpp"

#include "3rd/imgui/imgui.h"
#include "3rd/imgui/imgui_impl_glfw.h"
#include "3rd/imgui/imgui_impl_opengl3.h"

#include "debug.hpp"

#define WINDOW_WIDTH  1920.0
#define WINDOW_HEIGHT 1080.0

inline glm::vec3 camera_forward(const glm::vec3 &position, const glm::vec3 &target)
{
  return glm::normalize(position - target);
}

inline glm::vec3 camera_right(const glm::vec3 &position, const glm::vec3 &target)
{
  return glm::cross(camera_forward(position, target), glm::vec3(0.0, 1.0, 0.0));
}

inline glm::vec3 camera_up(const glm::vec3 &position, const glm::vec3 &target)
{
  const glm::vec3 f = camera_forward(position, target);
  const glm::vec3 r = glm::cross(f, glm::vec3(0.0, 1.0, 0.0));
  return glm::cross(f, r);
}

void imgui_setup(const ZD::Window_GLFW &window)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  //io.Fonts->AddFontFromFileTTF("./data/Roboto-Medium.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("./data/Roboto-Regular.ttf", 16.0f);

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window.get_handle(), true);
  ImGui_ImplOpenGL3_Init("#version 150");
}

void imgui_frame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void imgui_render()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void imgui_cleanup()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

int main()
{
  const ZD::Color sky_color { 225, 240, 255 };

  ZD::OGLRenderer renderer;
  renderer.set_multisampling(1);
  auto window = renderer.add_window(ZD::WindowParameters(ZD::Size(WINDOW_WIDTH, WINDOW_HEIGHT), "Mech"));
  //renderer.enable_cull_face();
  renderer.enable_depth_test(GL_LESS);
  renderer.enable_blend();
  renderer.clear_background_color(sky_color);

  Debug::init();

  imgui_setup(*static_cast<ZD::Window_GLFW *>(window.get()));

  auto ground = std::make_shared<Ground>();
  ground->set_fog_color(sky_color);

  auto mech = std::make_shared<Mech>(glm::vec3 { 2.0, 5.0, 0.0 });
  std::vector<Prop> props;
  for (ssize_t i = -20; i < 40; i++)
    for (ssize_t j = -20; j < 40; j++)
    {
      glm::vec3 pos { 0.0, -2.0, 0.0 };
      pos.x += i * 20.0;
      pos.z += j * 20.0;
      pos.y = ground->get_y(pos.x, pos.z);

      const auto n = ground->get_n(pos.x, pos.z);
      const float theta = glm::dot(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
      const float s = sqrt((1.0f + theta) * 2.0f);
      const glm::vec3 a = glm::cross(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
      auto rot = glm::quat(s * 0.5f, a.x * (1.0f / s), a.y * (1.0f / s), a.z * (1.0f / s));

      DBG("Props orientations", Debug::add_line(pos, pos + n * 20.0f));

      if (pos.y > -1.0 && pos.y < 1.4)
      {
        props.push_back(Prop { PropType::Rock, pos, rot, glm::vec3 { 1.0f } });
      }
      else
      {
        props.push_back(Prop { PropType::Tree, pos, rot, glm::vec3 { 1.0f } });
      }
    }

  Sky sky(sky_color);

  ZD::View view(
    ZD::Camera::PerspectiveParameters(
      ZD::Camera::Fov::from_degrees(100.0), WINDOW_WIDTH / WINDOW_HEIGHT, ZD::Camera::ClippingPlane(0.01, 2000.0)),
    glm::vec3(0.0, 0.0, 0.0));

  glm::vec3 camera_position { 0.0, 0.0, 0.0 };

  printf("Ready.\n");

  while (window->is_open())
  {
    renderer.clear();
    renderer.update();
    imgui_frame();

    //ImGui::Begin("Test");
    //ImGui::Text("ABCD");
    //ImGui::End();

    float CAMERA_STEP_SIZE = 1.0;
    if (window->input()->key(ZD::Key::LeftShift))
      CAMERA_STEP_SIZE *= 10.0;
    if (window->input()->key(ZD::Key::LeftControl))
      CAMERA_STEP_SIZE /= 10.0;

    if (window->input()->key(ZD::Key::W))
    {
      camera_position -= camera_forward(view.get_position(), mech->get_position()) * CAMERA_STEP_SIZE;
    }
    if (window->input()->key(ZD::Key::S))
    {
      camera_position += camera_forward(view.get_position(), mech->get_position()) * CAMERA_STEP_SIZE;
    }
    if (window->input()->key(ZD::Key::A))
    {
      camera_position += camera_right(view.get_position(), mech->get_position()) * CAMERA_STEP_SIZE;
    }
    if (window->input()->key(ZD::Key::D))
    {
      camera_position -= camera_right(view.get_position(), mech->get_position()) * CAMERA_STEP_SIZE;
    }
    if (window->input()->key(ZD::Key::E))
    {
      camera_position += camera_up(view.get_position(), mech->get_position()) * CAMERA_STEP_SIZE;
    }
    if (window->input()->key(ZD::Key::Q))
    {
      camera_position -= camera_up(view.get_position(), mech->get_position()) * CAMERA_STEP_SIZE;
    }

    const double camera_min_y = ground->get_y(camera_position.x, camera_position.z);
    if (camera_position.y - 5.0f < camera_min_y)
    {
      camera_position.y = camera_min_y + 5.0f;
    }

    view.set_position(camera_position);
    view.set_target(mech->get_position());
    sky.render(view);

    mech->render(view);
    for (auto &&prop : props)
    {
      // non transparent
      if (prop.type != PropType::Tree)
        prop.draw(view);
    }
    ground->draw(view);
    for (auto &&prop : props)
    {
      // transparent
      if (prop.type == PropType::Tree)
        prop.draw(view);
    }

    if (window->input()->mouse().consume_button(ZD::MouseButton::Left))
    {
      const glm::vec3 new_pos { camera_position.x,
                                ground->get_y(camera_position.x, camera_position.z),
                                camera_position.z };
      mech->set_position(new_pos);
      camera_position.x -= 4.0f;
      camera_position.z -= 4.0f;
    }

    Debug::draw_lines(view);

    imgui_render();

    renderer.render();
  }

  imgui_cleanup();
  window->kill();

  printf("Done.\n");

  return 0;
}
