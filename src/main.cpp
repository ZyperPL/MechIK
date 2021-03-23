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

#include "3rd/imgui/imgui.h"
#include "3rd/imgui/imgui_impl_glfw.h"
#include "3rd/imgui/imgui_impl_opengl3.h"

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
  ZD::OGLRenderer renderer;
  renderer.set_multisampling(1);
  auto window = renderer.add_window(ZD::WindowParameters(ZD::Size(WINDOW_WIDTH, WINDOW_HEIGHT), "Mech"));
  //renderer.enable_cull_face();
  renderer.enable_depth_test(GL_LESS);

  imgui_setup(*static_cast<ZD::Window_GLFW*>(window.get()));

  Mech *mech = new Mech { { 2.0, 5.0, 0.0 } };
  std::vector<Prop> props;
  for (size_t i = 0; i < 2; i++)
    for (size_t j = 0; j < 2; j++)
    {
      glm::vec3 pos { -40.0, -2.0, -20.0 };
      pos.x += i * 10.0;
      pos.z += j * 10.0;
      props.push_back(Prop { PropType::Tree, pos, { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 } });
    }

  ZD::View view(
    ZD::Camera::PerspectiveParameters(
      ZD::Camera::Fov::from_degrees(100.0), WINDOW_WIDTH / WINDOW_HEIGHT, ZD::Camera::ClippingPlane(0.01, 1000.0)),
    glm::vec3(0.0, 0.0, 0.0));

  auto ground = std::make_shared<Ground>();

  glm::vec3 camera_position { 0.0, 0.0, 0.0 };

  printf("Ready.\n");

  while (window->is_open())
  {
    renderer.clear();
    renderer.update();
    imgui_frame();
    
    ImGui::Begin("Test");
    ImGui::Text("ABCD");
    ImGui::End();

    float CAMERA_STEP_SIZE = 0.01;
    if (window->input()->key(ZD::Key::LeftShift))
      CAMERA_STEP_SIZE *= 10.0;
    if (window->input()->key(ZD::Key::LeftControl))
      CAMERA_STEP_SIZE *= 10.0;

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

    view.set_position(camera_position);
    view.set_target(mech->get_position());
    mech->render(view);
    for (auto &&prop : props)
    {
      prop.draw(*Mech::model_shader, view);
    }
    ground->draw(*ground->get_shader_program(), view);

    imgui_render();
    renderer.render();
  }
  
  imgui_cleanup();
  window->kill();

  printf("Done.\n");

  return 0;
}
