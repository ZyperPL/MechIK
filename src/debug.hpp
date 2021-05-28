#pragma once

#include "ZD/ShaderLoader.hpp"
#include "ZD/Entity.hpp"
#include "ZD/3rd/glm/gtc/type_ptr.hpp" // value_ptr
#include "ZD/3rd/glm/ext/matrix_transform.hpp" // translate, rotate, scale, identity
#include "ZD/Model.hpp"

#include <vector>
#include <algorithm>

class Mech;
struct Debug;

#ifdef DEBUG

#define DBG_ENABLE(k) Debug::enable(k)

#define DBG(k, x)        \
  if (Debug::enabled(k)) \
  {                      \
    x;                   \
  }

#else

#define DBG_ENABLE(k)
#define DBG(k, x)

#endif

struct Debug
{
  static void init()
  {
    glGenBuffers(1, &Debug::buffer);
    cube = ZD::Model::load(ZD::ModelDefault::Cube);

    Debug::shader = ZD::ShaderLoader()
                      .add(ZD::File("shaders/model.vertex.glsl"), GL_VERTEX_SHADER)
                      .add(ZD::File("shaders/model.fragment.glsl"), GL_FRAGMENT_SHADER)
                      .compile();
  }

  static void add_line(const glm::vec3 a, const glm::vec3 b) { lines.push_back({ a, b }); }
  static void add_cube(const std::string type, const glm::vec3 a) { cubes.push_back({ type, a }); }

  static void clear_cubes(const std::string type)
  {
    cubes.erase(
      std::remove_if(std::begin(cubes), std::end(cubes), [&type](auto &p) { return p.first == type; }),
      std::end(cubes));
  }

  static void generate_line_buffer()
  {
    std::vector<float> data;
    const size_t size = lines.size() * 3 * 2;
    data.reserve(size);

    for (const auto &line_pair : lines)
    {
      data.push_back(line_pair.first.x);
      data.push_back(line_pair.first.y);
      data.push_back(line_pair.first.z);
      data.push_back(line_pair.second.x);
      data.push_back(line_pair.second.y);
      data.push_back(line_pair.second.z);
    }
    assert(data.size() == size);

    glBindBuffer(GL_ARRAY_BUFFER, Debug::buffer);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
  }

  static void draw_lines(const ZD::View &view)
  {
    static size_t a = 1000;
    a++;
    if (a > 500)
    {
      generate_line_buffer();
      a = 0;
    }

    Debug::shader->use();

    const glm::mat4 model_matrix(1.0f);
    const glm::mat4 projection_matrix = view.get_projection_matrix();
    const glm::mat4 view_matrix = view.get_view_matrix();

    glUniformMatrix4fv(shader->get_uniform("V")->location, 1, GL_FALSE, glm::value_ptr(view_matrix));
    glUniformMatrix4fv(shader->get_uniform("M")->location, 1, GL_FALSE, glm::value_ptr(model_matrix));
    glUniformMatrix4fv(shader->get_uniform("P")->location, 1, GL_FALSE, glm::value_ptr(projection_matrix));

    const auto position_attribute = shader->get_attribute("position");
    glEnableVertexAttribArray(position_attribute->index);
    glBindBuffer(GL_ARRAY_BUFFER, Debug::buffer);
    glVertexAttribPointer(position_attribute->index, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glDrawArrays(GL_LINES, 0, lines.size() * 2);

    glDisableVertexAttribArray(position_attribute->index);
  }

  static void draw_cubes(const ZD::View &view)
  {
    Debug::shader->use();

    const glm::mat4 projection_matrix = view.get_projection_matrix();
    const glm::mat4 view_matrix = view.get_view_matrix();

    glUniformMatrix4fv(shader->get_uniform("V")->location, 1, GL_FALSE, glm::value_ptr(view_matrix));
    glUniformMatrix4fv(shader->get_uniform("P")->location, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    shader->set_uniform<bool>("debug", true);

    for (const auto &type_pos : cubes)
    {
      if (Debug::enabled(type_pos.first))
      {
        const glm::vec3 pos = type_pos.second;
        const glm::mat4 model_matrix =
          glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
        glUniformMatrix4fv(shader->get_uniform("M")->location, 1, GL_FALSE, glm::value_ptr(model_matrix));

        cube->draw(*shader);
      }
    }
  }

  static bool enabled(const std::string key)
  {
    return Debug::option.contains(key) ? Debug::option.at(key) : !Debug::option.insert_or_assign(key, false).second;
  }
  static void enable(const std::string key) { Debug::option.insert_or_assign(key, true); }
  static void disable(const std::string key) { Debug::option.insert_or_assign(key, false); }

  static std::unordered_map<std::string, bool> option;
  
  static void mech_properties_rotation(Mech &);
  static void mech_properties_position(Mech &);
  static void mech_properties_legs(Mech &);

private:
  static GLuint buffer;
  static std::shared_ptr<ZD::Model> cube;
  static std::vector<std::pair<glm::vec3, glm::vec3>> lines;
  static std::vector<std::pair<std::string, glm::vec3>> cubes;
  static std::unordered_map<std::string, bool> enabled_cubes;
  static std::shared_ptr<ZD::ShaderProgram> shader;
};
