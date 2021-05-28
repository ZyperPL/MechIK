#pragma once

#include "ZD/3rd/glm/glm.hpp"

#include "ZD/Entity.hpp"
#include "ZD/File.hpp"
#include "ZD/Model.hpp"
#include "ZD/ModelLoader.hpp"
#include "ZD/Shader.hpp"
#include "ZD/ShaderLoader.hpp"
#include "ZD/Texture.hpp"
#include "ZD/View.hpp"

struct World;

struct LegPart : public ZD::Entity
{
  LegPart(const size_t part_index);
  virtual ~LegPart() = default;

  const size_t part_index;
  glm::vec3 target_position { 0.0f, 0.0f, 0.0f };
};

class Mech : public ZD::Entity
{
public:
  Mech(glm::vec3 position);
  void update(const World &world);
  void render(ZD::View &view, const World &world);

  inline void set_path(std::vector<std::pair<int, int>> &&path) { this->path = path; }
  void set_legs_count(const size_t n);
  inline size_t get_legs_count() const { return legs_e.size(); }

  inline constexpr void set_height(const float v) { height = v; }
  inline constexpr float get_height() const { return height; }

  inline constexpr void set_move_speed(const float v) { move_speed = v; }
  inline constexpr float get_move_speed() const { return move_speed; }

  inline constexpr void set_rotation_speed(const float v) { rotation_speed = v; }
  inline constexpr float get_rotation_speed() const { return rotation_speed; }

  inline constexpr void set_angle_offset(const float v) { angle_offset = v; }
  inline constexpr float get_angle_offset() const { return angle_offset; }

private:
  std::shared_ptr<ZD::ShaderProgram> shader;

  std::unique_ptr<ZD::Entity> body;
  std::vector<std::unique_ptr<LegPart>> legs_b;
  std::vector<std::unique_ptr<LegPart>> legs_m;
  std::vector<std::unique_ptr<LegPart>> legs_e;
  std::vector<std::pair<int, int>> path;

  float height { 3.2f };
  float move_speed { 0.1f };
  float rotation_speed { 0.2f };
  float angle_offset { 1.0f };

  void step_path(const World &world);

  friend struct Debug;
};
