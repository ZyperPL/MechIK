#include "mech.hpp"

class LegEntity : public ZD::Entity {
public:
  LegEntity() {
    const size_t LEG_PAIRS = 2;
    for (size_t i = 0; i < LEG_PAIRS; i++) {
      auto leg_entity = std::make_unique<ZD::Entity>();
      leg_entity->add_model(
          std::make_shared<ZD::Model>("models/mech_leg_b.obj"));
      legs_b.push_back(std::move(leg_entity));

      leg_entity = std::make_unique<ZD::Entity>();
      leg_entity->add_model(
          std::make_shared<ZD::Model>("models/mech_leg_m.obj"));
      legs_m.push_back(std::move(leg_entity));

      leg_entity = std::make_unique<ZD::Entity>();
      leg_entity->add_model(
          std::make_shared<ZD::Model>("models/mech_leg_e.obj"));
      legs_e.push_back(std::move(leg_entity));
    }
  }

  void update_positions(const ZD::Entity &parent) {
    set_position(parent.get_position());
    set_rotation(parent.get_rotation());
    for (size_t i = 0; i < legs_b.size(); ++i)
    {
      auto &leg_b = legs_b[i];
      auto &leg_m = legs_m[i];
      auto &leg_e = legs_e[i];

      leg_b->set_position(get_position());
      leg_m->set_position(get_position());
      leg_e->set_position(get_position());
      
      leg_b->set_rotation(get_rotation());
      leg_m->set_rotation(get_rotation());
      leg_e->set_rotation(get_rotation());

      leg_b->move_position({0.0, -2.0, 0.0});
      leg_m->move_position({0.0, -4.0, 0.0});
      leg_e->move_position({0.0, -6.0, 0.0});
    }
  }

  void render(ZD::ShaderProgram &program, ZD::View &view)
  {
    for (size_t i = 0; i < legs_b.size(); ++i)
    {
      auto &leg_b = legs_b[i];
      auto &leg_m = legs_m[i];
      auto &leg_e = legs_e[i];

      leg_b->draw(program, view);
      leg_m->draw(program, view);
      leg_e->draw(program, view);

      printf("Drawing leg %lu.\n", i);
    }
  }

private:
  std::vector<std::shared_ptr<ZD::Entity>> legs_b;
  std::vector<std::shared_ptr<ZD::Entity>> legs_m;
  std::vector<std::shared_ptr<ZD::Entity>> legs_e;
};

Mech::Mech(glm::vec3 position)
    : ZD::Entity(position, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}) {

  body = std::make_unique<ZD::Entity>();
  body->add_model(std::make_shared<ZD::Model>("models/mech_body.obj"));
  body->set_position(position);

  legs.push_back(std::make_unique<LegEntity>());

  shader = ZD::ShaderLoader()
               .add(ZD::File("shaders/model.vertex.glsl"), GL_VERTEX_SHADER)
               .add(ZD::File("shaders/model.fragment.glsl"), GL_FRAGMENT_SHADER)
               .compile();
}

void Mech::render(ZD::View &view) {
  add_rotation({0.0, 0.01, 0.0});

  shader->use();
  body->set_position(get_position());
  body->set_rotation(get_rotation());
  body->set_scale(get_scale());
  body->draw(*shader, view);

  for (auto &&leg : legs) {
    leg->update_positions(*this);
    leg->render(*shader, view);
  }
}
