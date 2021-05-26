#include <map>
#include <unordered_set>
#include <set>
#include <memory>
#include <random>
#include <utility>

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
#include "config.hpp"
#include "gridmap.hpp"

#include "3rd/imgui/imgui.h"
#include "3rd/imgui/imgui_impl_glfw.h"
#include "3rd/imgui/imgui_impl_opengl3.h"
#include "ZD/3rd/glm/ext/matrix_projection.hpp"

#include "debug.hpp"

#include "world.hpp"

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
  ZD::OGLRenderer renderer;
  renderer.set_multisampling(1);
  auto window = renderer.add_window(ZD::WindowParameters(ZD::Size(WINDOW_WIDTH, WINDOW_HEIGHT), "Mech"));
  renderer.enable_blend();

  std::unique_ptr<World> world = std::make_unique<World>();
  renderer.clear_background_color(world->sky_color);

  Debug::init();

  std::shared_ptr<Config> cfg = std::make_shared<Config>("config.ini");

  imgui_setup(*static_cast<ZD::Window_GLFW *>(window.get()));
  ImGuiIO &imgui_io = ImGui::GetIO();

  world->ground = std::make_unique<Ground>();
  world->ground->set_fog_color(world->sky_color);
  world->generate();

  Sky sky(world->sky_color);

  ZD::View view(
    ZD::Camera::PerspectiveParameters(
      ZD::Camera::Fov::from_degrees(90.0), WINDOW_WIDTH / WINDOW_HEIGHT, ZD::Camera::ClippingPlane(0.1, 800.0)),
    glm::vec3(0.0, 0.0, 0.0));

  glm::vec3 camera_position { 0.0, 2.0, 0.0 };

  assert(world->mech);

  printf("Ready.\n");
  while (window->is_open())
  {
    renderer.clear();
    glClearColor(world->sky_color.red_float(), world->sky_color.green_float(), world->sky_color.blue_float(), 1.0);
    renderer.enable_depth_test(GL_LEQUAL);
    renderer.enable_cull_face();
    renderer.update();
    imgui_frame();
    world->mech->update(*world);

    if (ImGui::Begin("Debug options"))
    {
      ImGui::Text("Options available: %lu", Debug::option.size());
      for (auto &&option : Debug::option)
        ImGui::Checkbox(option.first.data(), &option.second);

      if (ImGui::Button("Show graph grid"))
      {
        for (auto &&idx_node : world->grid_map->nodes)
        {
          const float x = idx_node.first.first * world->X_SPACING;
          const float z = idx_node.first.second * world->Z_SPACING;

          const glm::vec3 pos { x, world->ground->get_y(x, z), z };
          Debug::add_cube(pos);
        }
      }
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
    {
      Debug::mech_properties(*world->mech);
      Debug::mech_debug(*world->mech);
    }
    ImGui::End();

    float CAMERA_STEP_SIZE = 1.0;
    if (window->input()->key(ZD::Key::LeftShift))
      CAMERA_STEP_SIZE *= 10.0;
    if (window->input()->key(ZD::Key::LeftControl))
      CAMERA_STEP_SIZE /= 10.0;

    if (window->input()->key(ZD::Key::W))
      camera_position -= camera_forward(view.get_position(), world->mech->get_position()) * CAMERA_STEP_SIZE;
    if (window->input()->key(ZD::Key::S))
      camera_position += camera_forward(view.get_position(), world->mech->get_position()) * CAMERA_STEP_SIZE;
    if (window->input()->key(ZD::Key::A))
      camera_position += camera_right(view.get_position(), world->mech->get_position()) * CAMERA_STEP_SIZE;
    if (window->input()->key(ZD::Key::D))
      camera_position -= camera_right(view.get_position(), world->mech->get_position()) * CAMERA_STEP_SIZE;
    if (window->input()->key(ZD::Key::E))
      camera_position += camera_up(view.get_position(), world->mech->get_position()) * CAMERA_STEP_SIZE;
    if (window->input()->key(ZD::Key::Q))
      camera_position -= camera_up(view.get_position(), world->mech->get_position()) * CAMERA_STEP_SIZE;

    if (!camera_noclip)
    {
      const double camera_min_y = world->ground->get_y(camera_position.x, camera_position.z);
      if (camera_position.y - 5.0f < camera_min_y)
      {
        camera_position.y = camera_min_y + 5.0f;
      }
    }

    view.set_position(camera_position);
    view.set_target(world->mech->get_position());

    sky.render(view);

    world->mech->render(view, *world);
    for (auto &&prop : world->props)
    {
      // non transparent
      if (!prop.has_transulency)
        prop.draw(view, *world);
    }
    world->ground->draw(view);
    for (auto &&prop : world->props)
    {
      // transparent
      if (prop.has_transulency)
        prop.draw(view, *world);
    }

    if (!imgui_io.WantCaptureMouse)
    {
      if (window->input()->mouse().consume_button(ZD::MouseButton::Left))
      {
        const auto mouse_position = window->input()->mouse().position();

        const glm::vec3 click_world_space = glm::unProject(
          glm::vec3(mouse_position.x, window->get_height() - mouse_position.y, 0.0f),
          view.get_view_matrix(),
          view.get_projection_matrix(),
          glm::vec4(0.0f, 0.0f, window->get_width(), window->get_height()));

        const glm::vec3 click_world_space_forward = glm::unProject(
          glm::vec3(mouse_position.x, window->get_height() - mouse_position.y, 1.0f),
          view.get_view_matrix(),
          view.get_projection_matrix(),
          glm::vec4(0.0f, 0.0f, window->get_width(), window->get_height()));

        const glm::vec3 click_direction_world_space = glm::normalize(click_world_space_forward - click_world_space);

        glm::vec3 p = click_world_space;
        const size_t MAX_RAY_STEPS = 1000;
        for (size_t i = 0; i < MAX_RAY_STEPS; i++)
        {
          p += click_direction_world_space * 3.0f;
          if (p.y < world->ground->get_y(p.x, p.z))
          {
            Debug::add_cube(p);
            Debug::add_cube(glm::vec3(p.x, p.y + 2.1f, p.z));
            Debug::add_cube(glm::vec3(p.x, p.y + 2.2f, p.z));
            Debug::add_cube(glm::vec3(p.x, p.y + 2.3f, p.z));
            Debug::add_cube(glm::vec3(p.x, p.y + 2.4f, p.z));

            const int end_x = p.x / world->X_SPACING;
            const int end_y = p.z / world->Z_SPACING;
            int start_x = world->mech->get_position().x / world->X_SPACING;
            int start_y = world->mech->get_position().z / world->Z_SPACING;

            size_t tries = 20;
            while (tries > 0 && !world->grid_map->nodes.contains({start_x, start_y}))
            {
              start_x = (start_x + 1);
              start_y = (start_y + 1);
              tries--;
            }

            auto path = world->grid_map->get_path(end_x, end_y, start_x, start_y);
            for (const auto &idx : path)
            {
              const float x = idx.first * world->X_SPACING;
              const float z = idx.second * world->Z_SPACING;
              const glm::vec3 pos { x, world->ground->get_y(x, z) + 3.0f, z };
              Debug::add_cube(pos);
            }
            world->mech->set_path(std::move(path));

            break;
          }
        }
      }
    }

    Debug::draw_lines(view);
    Debug::draw_cubes(view);

    renderer.render_screens();
    imgui_render();
    renderer.render();
  }

  imgui_cleanup();
  window->kill();

  printf("Done.\n");

  return 0;
}
