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

  static std::random_device rd;
  std::uniform_real_distribution<float> random(0.0f, 1.0f);

  auto mech = std::make_shared<Mech>(glm::vec3 { 2.0, 8.0, 10.0 });

  GridMap grid_map;
  std::set<std::pair<int, int>> bad_nodes;

  const float X_SPACING = 8.0f;
  const float Z_SPACING = 9.0f;

  const ssize_t MIN_X = -100;
  const ssize_t MAX_X = 100;
  const ssize_t MIN_Z = -100;
  const ssize_t MAX_Z = 100;

  for (ssize_t i = MIN_X; i < MAX_X; i++)
  {
    for (ssize_t j = MIN_Z; j < MAX_Z; j++)
    {
      glm::vec3 pos { 0.0, -2.0, 0.0 };
      pos.x += i * X_SPACING + (random(rd) - 0.5) * X_SPACING / 2.0f;
      pos.z += j * Z_SPACING + (random(rd) - 0.5) * Z_SPACING / 2.0f;
      pos.y = world->ground->get_y(pos.x, pos.z);

      auto &map_node = grid_map.add(static_cast<int>(i), static_cast<int>(j));

      auto n = world->ground->get_n(pos.x, pos.z);
      std::optional<PropType> added_prop;

      DBG("Props orientations", Debug::add_line(pos, pos + n * 20.0f));

      if (i % 3 == 0 && j % 3 == 0 && random(rd) > 0.8 && glm::distance(mech->get_position(), pos) > 5.0f)
      {
        if ((pos.y > 0.5 && pos.y < 2.0) || pos.y < -10.0)
        {
          const float theta = glm::dot(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
          const float s = sqrt((1.0f + theta) * 2.0f);
          const glm::vec3 a = glm::cross(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
          auto rot = glm::quat(s * 0.5f, a.x * (1.0f / s), a.y * (1.0f / s), a.z * (1.0f / s));
          world->props.push_back(Prop { PropType::Rock, pos, rot, glm::vec3 { 1.0f } });
          added_prop = PropType::Rock;
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
            world->props.push_back(Prop { PropType::Tree, pos, rot, glm::vec3 { 1.0f } });
            added_prop = PropType::Tree;
            if (random(rd) < 0.7)
            {
              gen_bush = true;
              pos.x += (random(rd) - 0.5) * 10.0;
              pos.z += (random(rd) - 0.5) * 10.0;
              pos.y = world->ground->get_y(pos.x, pos.z);

              n = world->ground->get_n(pos.x, pos.z);
            }
          }

          if (gen_bush)
          {
            pos -= n * 0.2f;
            const float theta = glm::dot(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
            const float s = sqrt((1.0f + theta) * 2.0f);
            const glm::vec3 a = glm::cross(glm::vec3 { 0.0f, 1.0f, 0.0f }, n);
            auto rot = glm::quat(s * 0.5f, a.x * (1.0f / s), a.y * (1.0f / s), a.z * (1.0f / s));
            if (random(rd) < 0.7)
            {
              world->props.push_back(Prop { PropType::Bush2, pos, rot, glm::vec3 { 1.0f } });
              if (!added_prop)
                added_prop = PropType::Bush2;
            }
            else
            {
              world->props.push_back(Prop { PropType::Bush1, pos, rot, glm::vec3 { 1.0f } });
              if (!added_prop)
                added_prop = PropType::Bush1;
            }
          }
        }
      }

      map_node.cost = GridMap::Node::calculate_cost(n, added_prop);
      if (map_node.cost > 0.99)
      {
        bad_nodes.insert({ map_node.x, map_node.y });
      }
    }
  }

  for (ssize_t i = MIN_X; i < MAX_X; i++)
  {
    for (ssize_t j = MIN_Z; j < MAX_Z; j++)
    {
      const std::pair<int, int> key { i, j };

      if (bad_nodes.contains(key))
      {
        bad_nodes.erase(key);
        grid_map.nodes.erase(key);
        const int REMOVE_R = 1;
        for (int iy = -REMOVE_R; iy <= REMOVE_R; ++iy)
        {
          for (int ix = -REMOVE_R; ix <= REMOVE_R; ++ix)
          {
            grid_map.nodes.erase({ i + ix, j + iy });
          }
        }
      }
    }
  }
  Sky sky(world->sky_color);

  ZD::View view(
    ZD::Camera::PerspectiveParameters(
      ZD::Camera::Fov::from_degrees(90.0), WINDOW_WIDTH / WINDOW_HEIGHT, ZD::Camera::ClippingPlane(0.1, 800.0)),
    glm::vec3(0.0, 0.0, 0.0));

  glm::vec3 camera_position { 0.0, 2.0, 0.0 };

  printf("Ready.\n");
  while (window->is_open())
  {
    renderer.clear();
    glClearColor(world->sky_color.red_float(), world->sky_color.green_float(), world->sky_color.blue_float(), 1.0);
    renderer.enable_depth_test(GL_LEQUAL);
    renderer.enable_cull_face();
    renderer.update();
    imgui_frame();
    mech->update(*world);

    if (ImGui::Begin("Debug options"))
    {
      ImGui::Text("Options available: %lu", Debug::option.size());
      for (auto &&option : Debug::option)
        ImGui::Checkbox(option.first.data(), &option.second);

      if (ImGui::Button("Show graph grid"))
      {
        for (auto &&idx_node : grid_map.nodes)
        {
          const float x = idx_node.first.first * X_SPACING;
          const float z = idx_node.first.second * Z_SPACING;

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
      Debug::mech_properties(*mech);
      Debug::mech_debug(*mech);
    }
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
      const double camera_min_y = world->ground->get_y(camera_position.x, camera_position.z);
      if (camera_position.y - 5.0f < camera_min_y)
      {
        camera_position.y = camera_min_y + 5.0f;
      }
    }

    view.set_position(camera_position);
    view.set_target(mech->get_position());

    sky.render(view);

    mech->render(view, *world);
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

            const int end_x = p.x / X_SPACING;
            const int end_y = p.z / Z_SPACING;
            const int start_x = mech->get_position().x / X_SPACING;
            const int start_y = mech->get_position().z / Z_SPACING;
            const auto path = grid_map.get_path(end_x, end_y, start_x, start_y);
            for (const auto &idx : path)
            {
              const float x = idx.first * X_SPACING;
              const float z = idx.second * Z_SPACING;
              const glm::vec3 pos { x, world->ground->get_y(x, z) + 3.0f, z };
              Debug::add_cube(pos);
            }

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
