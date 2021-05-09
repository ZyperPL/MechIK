#pragma once

#include "ZD/ShaderLoader.hpp"
#include "ZD/Entity.hpp"
#include "ZD/3rd/glm/gtc/type_ptr.hpp" // value_ptr

#include <vector>
#include <algorithm>

class Mech;
struct Debug;

#ifdef DEBUG
#define DBG(k, x)        \
  if (Debug::enabled(k)) \
  {                      \
    x;                   \
  }
#else
#define DBG(k, x)
#endif

struct Debug
{
  static void init()
  {
    glGenBuffers(1, &Debug::buffer);
    glGenBuffers(1, &Debug::cube_buffer);

    Debug::shader = ZD::ShaderLoader()
                      .add(ZD::File("shaders/model.vertex.glsl"), GL_VERTEX_SHADER)
                      .add(ZD::File("shaders/model.fragment.glsl"), GL_FRAGMENT_SHADER)
                      .compile();
  }

  static void add_line(const glm::vec3 a, const glm::vec3 b) { lines.push_back({ a, b }); }
  static void add_cube(const glm::vec3 a) { cubes.push_back(a); }

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

  static void generate_cube_buffer()
  {
    std::vector<GLfloat> data;
    const float U = 0.3f;
    data.push_back(-U);
    data.push_back(U);
    data.push_back(-U);

    data.push_back(U);
    data.push_back(U);
    data.push_back(-U);

    data.push_back(U);
    data.push_back(U);
    data.push_back(U);
    
    
    data.push_back(-U);
    data.push_back(U);
    data.push_back(-U);
    
    data.push_back(-U);
    data.push_back(U);
    data.push_back(U);
    
    data.push_back(U);
    data.push_back(U);
    data.push_back(U);

    glBindBuffer(GL_ARRAY_BUFFER, Debug::cube_buffer);
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
    static size_t a = 1000;
    a++;
    if (a > 500)
    {
      generate_cube_buffer();
      a = 0;
    }

    Debug::shader->use();

    const glm::mat4 projection_matrix = view.get_projection_matrix();
    const glm::mat4 view_matrix = view.get_view_matrix();

    glUniformMatrix4fv(shader->get_uniform("V")->location, 1, GL_FALSE, glm::value_ptr(view_matrix));
    glUniformMatrix4fv(shader->get_uniform("P")->location, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    const auto position_attribute = shader->get_attribute("position");
    glEnableVertexAttribArray(position_attribute->index);
    glBindBuffer(GL_ARRAY_BUFFER, Debug::cube_buffer);
    glVertexAttribPointer(position_attribute->index, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    shader->set_uniform<bool>("debug", true);

    for (const auto &cp : cubes)
    {
      const glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), cp);
      glUniformMatrix4fv(shader->get_uniform("M")->location, 1, GL_FALSE, glm::value_ptr(model_matrix));

      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glDisableVertexAttribArray(position_attribute->index);
  }

  static bool enabled(const std::string key)
  {
    return Debug::option.contains(key) ? Debug::option.at(key) : !Debug::option.insert_or_assign(key, false).second;
  }
  static void enable(const std::string key) { Debug::option.insert_or_assign(key, true); }
  static void disable(const std::string key) { Debug::option.insert_or_assign(key, false); }

  static std::unordered_map<std::string, bool> option;

  static void mech_properties(Mech &);
  static void mech_debug(Mech &);

private:
  static GLuint buffer, cube_buffer;
  static std::vector<std::pair<glm::vec3, glm::vec3>> lines;
  static std::vector<glm::vec3> cubes;
  static std::shared_ptr<ZD::ShaderProgram> shader;
};
