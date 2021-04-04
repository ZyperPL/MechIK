#include "debug.hpp"

std::vector<std::pair<glm::vec3, glm::vec3>> Debug::lines;
GLuint Debug::buffer = 0;
std::shared_ptr<ZD::ShaderProgram> Debug::shader;
std::unordered_map<std::string, bool> Debug::option;
