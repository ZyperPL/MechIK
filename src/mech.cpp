#include "mech.hpp"

#include "ZD/3rd/glm/gtx/rotate_vector.hpp"

class LegEntity
{
public:
  LegEntity()
  {
    const size_t LEG_PAIRS = 1;
    for (size_t i = 0; i < LEG_PAIRS * 2; i++)
    {
      auto leg_entity = std::make_unique<ZD::Entity>();
      leg_entity->add_model(std::make_shared<ZD::Model>("models/mech_leg_b.obj"));
      legs_b.push_back(std::move(leg_entity));

      leg_entity = std::make_unique<ZD::Entity>();
      leg_entity->add_model(std::make_shared<ZD::Model>("models/mech_leg_m.obj"));
      legs_m.push_back(std::move(leg_entity));

      leg_entity = std::make_unique<ZD::Entity>();
      leg_entity->add_model(std::make_shared<ZD::Model>("models/mech_leg_e.obj"));
      legs_e.push_back(std::move(leg_entity));
    }
  }

  void update_positions(const ZD::Entity &parent)
  {
    glm::vec3 position = parent.get_position();
    static float a = 0.0;
    a += 0.1;
    const float r1 = sin(a) * 0.1;
    const float r2 = cos(a / 1.2 + 0.3) * 0.1;

    for (size_t i = 0; i < legs_b.size(); ++i)
    {
      glm::vec3 rotation = parent.get_rotation();
      //rotation.y += M_PI / 2.0 + M_PI / (legs_b.size() * 2.0);
      //rotation.y += M_PI / 12.0;

      if ((i % 2) == 0)
        rotation.y = -M_PI - rotation.y;

      auto &leg_b = legs_b[i];
      auto &leg_m = legs_m[i];
      auto &leg_e = legs_e[i];

      leg_b->set_position(position);
      leg_m->set_position(position);
      leg_e->set_position(position);

      leg_b->set_rotation(rotation);
      leg_m->set_rotation(rotation);
      leg_e->set_rotation(rotation);

      leg_b->move_position({ 0.0, -1.2, 0.0 });
      leg_m->move_position({ 0.0, -1.2, 0.0 });

      printf("%f\n", rotation.y);

      glm::vec3 m_pos { 1.0, 0.0, 0.0 };
      m_pos = glm::rotate(m_pos, rotation.y, glm::vec3(0.0, 1.0, 0.0));
      leg_m->move_position(m_pos);
      leg_m->add_rotation(glm::rotate({ 0.0, 0.0, -r1 }, rotation.y, glm::vec3(0.0, 1.0, 0.0)));

      m_pos = glm::vec3(1.0, -1.3, 0.0);
      m_pos += glm::rotate(glm::vec3(1.0, 0.0, 0.0), -r1, { 0.0f, 0.0f, 1.0f });
      //m_pos += glm::rotate(glm::vec3(0.0, 1.0, 0.0), -r, {0.0f, 0.0f, 1.0f});
      m_pos = glm::rotate(m_pos, rotation.y, glm::vec3(0.0, 1.0, 0.0));
      leg_e->move_position(m_pos);
      leg_e->add_rotation(glm::rotate({ 0.0, 0.0, -r2 }, rotation.y, glm::vec3(0.0, 1.0, 0.0)));
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
    }
  }

private:
  std::vector<std::shared_ptr<ZD::Entity>> legs_b;
  std::vector<std::shared_ptr<ZD::Entity>> legs_m;
  std::vector<std::shared_ptr<ZD::Entity>> legs_e;
};

Mech::Mech(glm::vec3 position)
: ZD::Entity(position, { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 })
{
  body = std::make_unique<ZD::Entity>();
  body->add_model(std::make_shared<ZD::Model>("models/mech_body.obj"));
  body->set_position(position);

  legs.push_back(std::make_unique<LegEntity>());

  shader = ZD::ShaderLoader()
             .add(ZD::File("shaders/model.vertex.glsl"), GL_VERTEX_SHADER)
             .add(ZD::File("shaders/model.fragment.glsl"), GL_FRAGMENT_SHADER)
             .compile();
}

void Mech::render(ZD::View &view)
{
  shader->use();
  body->set_position(get_position());
  body->set_rotation(get_rotation());
  body->set_scale(get_scale());
  body->draw(*shader, view);

  for (auto &&leg : legs)
  {
    leg->update_positions(*this);
    leg->render(*shader, view);
  }
}
