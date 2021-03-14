#include "ZD/Entity.hpp"
#include "ZD/Input.hpp"
#include "ZD/Model.hpp"
#include "ZD/ModelLoader.hpp"
#include "ZD/OpenGLRenderer.hpp"
#include "ZD/ShaderLoader.hpp"
#include "ZD/Window.hpp"

#include "mech.hpp"

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

int main()
{
  ZD::OGLRenderer renderer;
  renderer.set_multisampling(16);
  auto window = renderer.add_window(ZD::WindowParameters(ZD::Size(WINDOW_WIDTH, WINDOW_HEIGHT), "Mech"));
  renderer.enable_cull_face();
  renderer.enable_depth_test(GL_LESS);

  Mech *mech = new Mech { { 5.0, 0.0, 0.0 } };

  ZD::View view(
    ZD::Camera::PerspectiveParameters(
      ZD::Camera::Fov::from_degrees(100.0), WINDOW_WIDTH / WINDOW_HEIGHT, ZD::Camera::ClippingPlane(0.1, 100.0)),
    glm::vec3(0.0, 0.0, 0.0));

  glm::vec3 camera_position { 0.0, 0.0, 0.0 };

  printf("Ready.\n");

  while (window->is_open())
  {
    renderer.clear();
    renderer.update();

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

    view.set_position(camera_position);
    view.set_target(mech->get_position());
    mech->render(view);

    renderer.render();
  }

  window->kill();

  printf("Done.\n");

  return 0;
}
