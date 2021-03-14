#include "ZD/Entity.hpp"
#include "ZD/Input.hpp"
#include "ZD/Model.hpp"
#include "ZD/ModelLoader.hpp"
#include "ZD/OpenGLRenderer.hpp"
#include "ZD/ShaderLoader.hpp"
#include "ZD/Window.hpp"

#include "mech.hpp"

#define W 1920.
#define H 1080.

int main() {
  ZD::OGLRenderer renderer;
  renderer.set_multisampling(16);
  auto window =
      renderer.add_window(ZD::WindowParameters(ZD::Size(W, H), "Mech"));
  renderer.enable_cull_face();
  renderer.enable_depth_test();

  Mech *mech = new Mech{{5.0, 0.0, 0.0}};

  ZD::View view(ZD::Camera::PerspectiveParameters(
                    ZD::Camera::Fov::from_degrees(100.0), W / H,
                    ZD::Camera::ClippingPlane(0.1, 1000.0)),
                glm::vec3(0.0, 0.0, 0.0));
  printf("Ready.\n");

  while (window->is_open()) {
    renderer.clear();
    renderer.update();

    view.set_target(mech->get_position());
    mech->render(view);

    renderer.render();
  }

  window->kill();

  printf("Done.\n");

  return 0;
}
