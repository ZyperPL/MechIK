#include "ZD/Input.hpp"
#include "ZD/Entity.hpp"
#include "ZD/Model.hpp"
#include "ZD/ModelLoader.hpp"
#include "ZD/OpenGLRenderer.hpp"
#include "ZD/ShaderLoader.hpp"
#include "ZD/Window.hpp"

int main() {
  ZD::OGLRenderer renderer;
  renderer.set_multisampling(16);
  auto window =
      renderer.add_window(ZD::WindowParameters(ZD::Size(800, 600), "Mech"));
  renderer.enable_cull_face();
  renderer.enable_depth_test();

  ZD::Model mech_body_model("models/mech_body.obj");
  ZD::Entity entity;
  entity.add_model(mech_body_model);
  entity.set_position(glm::vec3(5.0, 0.0, 0.0));

  auto shader =
      ZD::ShaderLoader()
          .add(ZD::File("shaders/model.vertex.glsl"), GL_VERTEX_SHADER)
          .add(ZD::File("shaders/model.fragment.glsl"), GL_FRAGMENT_SHADER)
          .compile();

  ZD::View view(ZD::Camera::PerspectiveParameters(
                    ZD::Camera::Fov::from_degrees(100.0), 800.0 / 600.0,
                    ZD::Camera::ClippingPlane(0.1, 1000.0)),
                glm::vec3(0.0, 0.0, 0.0));
  printf("Ready.\n");

  float a = 0.0, b = 0.0, c = 0.0;

  while (window->is_open()) {
    renderer.clear();
    renderer.update();

    if (window->input()->key(ZD::Key::A)) a += 0.01;
    if (window->input()->key(ZD::Key::Z)) a -= 0.01;
    if (window->input()->key(ZD::Key::S)) b += 0.01;
    if (window->input()->key(ZD::Key::X)) b -= 0.01;
    if (window->input()->key(ZD::Key::D)) c += 0.01;
    if (window->input()->key(ZD::Key::C)) c -= 0.01;
    printf("%f %f %f\n", a, b, c);

    entity.set_rotation(glm::vec3(a, b, c));
    view.set_target(entity.get_position());
    entity.draw(*shader, view);

    renderer.render();
  }

  printf("Done.\n");

  return 0;
}
