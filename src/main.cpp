#include "ZD/OpenGLRenderer.hpp"
#include "ZD/ShaderLoader.hpp"
#include "ZD/Model.hpp"
#include "ZD/ModelLoader.hpp"
#include "ZD/Entity.hpp"
#include "ZD/Window.hpp"

int main()
{
  ZD::OGLRenderer renderer;
  auto window = renderer.add_window(ZD::WindowParameters(ZD::Size(800, 600), "Mech"));

  ZD::Model mech_body_model("models/mech_body.obj");
  ZD::Entity entity;
  entity.add_model(mech_body_model);
  entity.set_position(glm::vec3(5.0, 0.0, 0.0));

  auto shader = ZD::ShaderLoader().add(ZD::ShaderDefault::CenterModelTextureVertex, GL_VERTEX_SHADER).add(ZD::ShaderDefault::CenterModelTextureFragment, GL_FRAGMENT_SHADER).compile();

  ZD::View view(ZD::Camera::PerspectiveParameters(ZD::Camera::Fov::from_degrees(100.0), 800.0/600.0, ZD::Camera::ClippingPlane(0.1, 1000.0)), glm::vec3(0.0, 0.0, 0.0));
  printf("Ready.\n");

  while (window->is_open())
  {
    renderer.clear();
    renderer.update();
    
    view.set_target(entity.get_position());
    entity.draw(*shader, view);

    renderer.render();
  }

  printf("Done.\n");

  return 0;
}
