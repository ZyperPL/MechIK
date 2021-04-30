#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>

class Config final
{
public:
  Config(const std::string file_name);

private:
  const std::string file_name;
};
