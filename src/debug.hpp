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

  static void add_line(const std::string type, const glm::vec3 a, const glm::vec3 b)
  {
    lines.push_back({ type, { a, b } });
  }

  static void clear_lines(const std::string type)
  {
    lines.erase(
      std::remove_if(std::begin(lines), std::end(lines), [&type](auto &p) { return p.first == type; }),
      std::end(lines));
  }

  static void add_cube(const std::string type, const glm::vec3 a) { cubes.push_back({ type, a }); }

  static void clear_cubes(const std::string type)
  {
    cubes.erase(
      std::remove_if(std::begin(cubes), std::end(cubes), [&type](auto &p) { return p.first == type; }),
      std::end(cubes));
  }

  static GLuint generate_line_buffer()
  {
    //TODO: rewrite to only use 2 vertices
    std::vector<float> data;
    const size_t size = lines.size() * 3 * 2;

    for (const auto &type_line_pair : lines)
    {
      const auto &type = type_line_pair.first;
      const auto &line_pair = type_line_pair.second;

      if (Debug::enabled(type))
      {
        data.push_back(line_pair.first.x);
        data.push_back(line_pair.first.y);
        data.push_back(line_pair.first.z);
        data.push_back(line_pair.second.x);
        data.push_back(line_pair.second.y);
        data.push_back(line_pair.second.z);
      }
    }
    assert(data.size() <= size);

    if (data.size() <= 1)
      return 0;

    glBindBuffer(GL_ARRAY_BUFFER, Debug::buffer);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);

    return data.size();
  }

  static void draw_lines(const ZD::View &view)
  {
    if (lines.size() < 2)
      return;

    static size_t gen_counter = 1000;
    static size_t lines_buffer_size = 0;
    if (gen_counter++ > 30)
    {
      gen_counter = 0;
      lines_buffer_size = generate_line_buffer();
    }
    if (lines_buffer_size < 2)
      return;

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
        float scale = 0.2f;
        if (type_pos.first == "Path" || type_pos.first == "Grid")
        {
          const float dst = glm::distance(view.get_position(), type_pos.second);
          scale = 0.1f + dst * 0.004f;
        }

        const glm::vec3 pos = type_pos.second;
        const glm::mat4 model_matrix =
          glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * scale);
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
  static std::vector<std::pair<std::string, std::pair<glm::vec3, glm::vec3>>> lines;
  static std::vector<std::pair<std::string, glm::vec3>> cubes;
  static std::unordered_map<std::string, bool> enabled_cubes;
  static std::shared_ptr<ZD::ShaderProgram> shader;
};
