#include <memory>
#include <random>

#include "ZD/Entity.hpp"
#include "ZD/Input.hpp"
#include "ZD/Model.hpp"
#include "ZD/ModelLoader.hpp"
#include "ZD/OpenGLRenderer.hpp"
#include "ZD/ShaderLoader.hpp"
#include "ZD/Window.hpp"
#include "ZD/Screen.hpp"

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

  io.Fonts->AddFontFromFileTTF("fonts/iosevka-regular.ttf", 16.0f);

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
  renderer.enable_blend();
  renderer.clear_background_color(sky_color);

  Debug::init();

  imgui_setup(*static_cast<ZD::Window_GLFW *>(window.get()));
  ImGuiIO &imgui_io = ImGui::GetIO();

  auto ground = std::make_shared<Ground>();
  ground->set_fog_color(sky_color);

  static std::random_device rd;
  std::uniform_real_distribution<float> random(0.0f, 1.0f);

  auto mech = std::make_shared<Mech>(glm::vec3 { 4.0, 8.0, 3.0 });
  std::vector<Prop> props;
  for (ssize_t i = -5; i < 6; i++)
    for (ssize_t j = -5; j < 6; j++)
    {
      glm::vec3 pos { 0.0, -2.0, 0.0 };
      pos.x += i * 42.0 + (random(rd)-0.5) * 15.0;
      pos.z += j * 44.0 + (random(rd)-0.5) * 21.0;
      pos.y = ground->get_y(pos.x, pos.z);

      auto n = ground->get_n(pos.x, pos.z);

      DBG("Props orientations", Debug::add_line(pos, pos + n * 20.0f));

      if ((pos.y > 0.5 && pos.y < 2.0) || pos.y < -10.0)
      {
        const float theta = glm::dot(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
        const float s = sqrt((1.0f + theta) * 2.0f);
        const glm::vec3 a = glm::cross(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
        auto rot = glm::quat(s * 0.5f, a.x * (1.0f / s), a.y * (1.0f / s), a.z * (1.0f / s));
        props.push_back(Prop { PropType::Rock, pos, rot, glm::vec3 { 1.0f } });
      }
      else
      {
        bool gen_bush = false;
        if (random(rd) < 0.3)
          gen_bush = true;

        if (!gen_bush)
        {
          pos -= n * 1.0f;
          n = glm::normalize(glm::vec3 { n.x, n.y * 8.0f, n.z });
          const float theta = glm::dot(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
          const float s = sqrt((1.0f + theta) * 2.0f);
          const glm::vec3 a = glm::cross(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
          auto rot = glm::quat(s * 0.5f, a.x * (1.0f / s), a.y * (1.0f / s), a.z * (1.0f / s));
          props.push_back(Prop { PropType::Tree, pos, rot, glm::vec3 { 1.0f } });
          if (random(rd) < 0.7)
          {
            gen_bush = true;
            pos.x += (random(rd) - 0.5) * 10.0;
            pos.z += (random(rd) - 0.5) * 10.0;
            pos.y = ground->get_y(pos.x, pos.z);
          }
        }

        if (gen_bush)
        {
          const float theta = glm::dot(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
          const float s = sqrt((1.0f + theta) * 2.0f);
          const glm::vec3 a = glm::cross(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
          auto rot = glm::quat(s * 0.5f, a.x * (1.0f / s), a.y * (1.0f / s), a.z * (1.0f / s));
          if (random(rd) < 0.7)
          {
            props.push_back(Prop { PropType::Bush2, pos, rot, glm::vec3 { 1.0f } });
          } else
          {
            props.push_back(Prop { PropType::Bush1, pos, rot, glm::vec3 { 1.0f } });
          }
        }
      }
    }

  Sky sky(sky_color);

  const double FAR_PLANE = 400.0;

  ZD::View view(
    ZD::Camera::PerspectiveParameters(
      ZD::Camera::Fov::from_degrees(100.0),
      WINDOW_WIDTH / WINDOW_HEIGHT,
      ZD::Camera::ClippingPlane(1.0, FAR_PLANE + 10.0)),
    glm::vec3(0.0, 0.0, 0.0));

  ZD::View view_bg(
    ZD::Camera::PerspectiveParameters(
      ZD::Camera::Fov::from_degrees(100.0),
      WINDOW_WIDTH / WINDOW_HEIGHT,
      ZD::Camera::ClippingPlane(FAR_PLANE - 10.0, 2800.0)),
    glm::vec3(0.0, 0.0, 0.0));

  glm::vec3 camera_position { 0.0, 2.0, 0.0 };

  auto sky_fb = renderer.generate_framebuffer(
    WINDOW_WIDTH, WINDOW_HEIGHT, ZD::TextureParameters { .mag_filter = GL_LINEAR, .min_filter = GL_LINEAR });
  window->add_screen(std::make_shared<ZD::Screen_GL>(sky_fb.texture, 0, 0));
  window->get_screens().back()->flip_y = true;

  auto background_fb = renderer.generate_framebuffer(
    WINDOW_WIDTH, WINDOW_HEIGHT, ZD::TextureParameters { .mag_filter = GL_LINEAR, .min_filter = GL_LINEAR });
  window->add_screen(std::make_shared<ZD::Screen_GL>(background_fb.texture, 0, 0));
  window->get_screens().back()->flip_y = true;

  auto main_fb = renderer.generate_framebuffer(
    WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2, ZD::TextureParameters { .mag_filter = GL_LINEAR, .min_filter = GL_LINEAR });
  window->add_screen(std::make_shared<ZD::Screen_GL>(main_fb.texture));
  window->get_screens().back()->flip_y = true;

  printf("Ready.\n");
  while (window->is_open())
  {
    renderer.clear();
    glClearColor(sky_color.red_float(), sky_color.green_float(), sky_color.blue_float(), 1.0);
    renderer.enable_depth_test(GL_LEQUAL);
    //renderer.enable_cull_face();
    renderer.update();
    imgui_frame();

    if (ImGui::Begin("Debug options"))
    {
      ImGui::Text("Options available: %lu", Debug::option.size());
      for (auto &&option : Debug::option)
        ImGui::Checkbox(option.first.data(), &option.second);
    }
    ImGui::End();

    static bool camera_noclip = false;
    if (ImGui::Begin("Camera"))
    {
      ImGui::Text("Camera position: %6.4f, %6.4f, %6.4f", camera_position.x, camera_position.y, camera_position.z);
      if (ImGui::Button("Set origin to center"))
        camera_position = glm::vec3 { 0.0f, 0.0f, 0.0f };

      ImGui::Checkbox("Noclip", &camera_noclip);
    }
    ImGui::End();

    if (ImGui::Begin("Mech"))
      Debug::mech_properties(*mech);
    ImGui::End();

    float CAMERA_STEP_SIZE = 1.0;
    if (window->input()->key(ZD::Key::LeftShift))
      CAMERA_STEP_SIZE *= 10.0;
    if (window->input()->key(ZD::Key::LeftControl))
      CAMERA_STEP_SIZE /= 10.0;

    if (window->input()->key(ZD::Key::W))
      camera_position -= camera_forward(view.get_position(), mech->get_position()) * CAMERA_STEP_SIZE;
    if (window->input()->key(ZD::Key::S))
      camera_position += camera_forward(view.get_position(), mech->get_position()) * CAMERA_STEP_SIZE;
    if (window->input()->key(ZD::Key::A))
      camera_position += camera_right(view.get_position(), mech->get_position()) * CAMERA_STEP_SIZE;
    if (window->input()->key(ZD::Key::D))
      camera_position -= camera_right(view.get_position(), mech->get_position()) * CAMERA_STEP_SIZE;
    if (window->input()->key(ZD::Key::E))
      camera_position += camera_up(view.get_position(), mech->get_position()) * CAMERA_STEP_SIZE;
    if (window->input()->key(ZD::Key::Q))
      camera_position -= camera_up(view.get_position(), mech->get_position()) * CAMERA_STEP_SIZE;

    if (!camera_noclip)
    {
      const double camera_min_y = ground->get_y(camera_position.x, camera_position.z);
      if (camera_position.y - 5.0f < camera_min_y)
      {
        camera_position.y = camera_min_y + 5.0f;
      }
    }

    view.set_position(camera_position);
    view.set_target(mech->get_position());

    view_bg.set_position(camera_position);
    view_bg.set_target(mech->get_position());

    renderer.bind_framebuffer(sky_fb);
    glClearColor(sky_color.red_float(), sky_color.green_float(), sky_color.blue_float(), 0.0);
    sky.render(view_bg);
    sky.render(view);
    renderer.unbind_framebuffer();

    renderer.bind_framebuffer(background_fb);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    ground->draw(view_bg);
    mech->render(view_bg);
    renderer.unbind_framebuffer();

    renderer.bind_framebuffer(main_fb);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    mech->render(view);
    for (auto &&prop : props)
    {
      // non transparent
      if (!prop.has_transulency)
        prop.draw(view);
    }
    ground->draw(view);
    for (auto &&prop : props)
    {
      // transparent
      if (prop.has_transulency)
        prop.draw(view);
    }

    if (!imgui_io.WantCaptureMouse)
    {
      if (window->input()->mouse().consume_button(ZD::MouseButton::Left))
      {
        const glm::vec3 new_pos { camera_position.x,
                                  ground->get_y(camera_position.x, camera_position.z) + 5.0f,
                                  camera_position.z };
        mech->set_position(new_pos);
        camera_position.x -= 4.0f;
        camera_position.z -= 4.0f;
      }
    }

    Debug::draw_lines(view);
    renderer.unbind_framebuffer();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    renderer.disable_depth_test();
    //renderer.disable_cull_face();

    renderer.render_screens();
    imgui_render();
    renderer.render();
  }

  imgui_cleanup();
  window->kill();

  printf("Done.\n");

  return 0;
}
