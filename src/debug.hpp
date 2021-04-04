#pragma once

#include "ZD/ShaderLoader.hpp"
#include "ZD/Entity.hpp"
#include "ZD/3rd/glm/gtc/type_ptr.hpp" // value_ptr

#include <vector>
#include <algorithm>

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

    Debug::shader = ZD::ShaderLoader()
                      .add(ZD::File("shaders/model.vertex.glsl"), GL_VERTEX_SHADER)
                      .add(ZD::File("shaders/model.fragment.glsl"), GL_FRAGMENT_SHADER)
                      .compile();
  }

  static void add_line(const glm::vec3 a, const glm::vec3 b) { lines.push_back({ a, b }); }

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
    if (a > 1000)
    {
      generate_line_buffer();
      a = 0;
    }
    a++;

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

  static bool enabled(const std::string key) { return Debug::option.contains(key) ? Debug::option.at(key) : false; }

private:
  static GLuint buffer;
  static std::vector<std::pair<glm::vec3, glm::vec3>> lines;
  static std::shared_ptr<ZD::ShaderProgram> shader;
  static std::unordered_map<std::string, bool> option;
};
