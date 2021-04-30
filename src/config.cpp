#include "config.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ZD/File.hpp"

Config::Config(const std::string file_name)
{
  const ZD::File config_file(file_name);
  const auto fd = config_file.get_fd();

  enum
  {
    Empty,
    Section,
    Key,
    Value,
  } mode = Empty;

  enum
  {
    World,
    Prop
  } section = World;

  const auto set_section = [&section](std::string &k) {
    if (k.starts_with('W'))
      section = World;
    else if (k.starts_with('P'))
      section = Prop;

    k = "";
  };

  struct PropValues
  {
    std::unordered_map<std::string, std::string> values;
  };
  std::vector<PropValues> props_config;
  struct
  {
    std::unordered_map<std::string, std::string> values;
  } world_config;

  std::string key {}, value {};

  auto add_key_value = [&section, &props_config, &world_config](const std::string key, const std::string value) {
    if (section == World)
      world_config.values.insert({ key, value });
    else if (section == Prop)
      props_config.back().values.insert({ key, value });
  };

  char ch;
  while (read(fd, &ch, sizeof(char)) > 0)
  {
    switch (mode)
    {
      case Empty:
        if (isspace(ch))
          continue;
        if (ch == '[')
        {
          mode = Section;
          continue;
        }
        
        mode = Key;
        key += ch;
      break;
      case Section:
        if (isspace(ch))
          continue;

        if (ch == ']')
        {
          set_section(key);
          mode = Key;

          if (section == Prop)
            props_config.push_back({});
        }
        else
          key += ch;
        break;
      case Key:
        if (isspace(ch))
          continue;

        if (ch == '=')
          mode = Value;
        else
          key += ch;
        break;
      case Value:
        if (ch == '\n')
        {
          add_key_value(key, value);
          key = "";
          value = "";
          mode = Empty;
        }
        else
        {
          value += ch;
        }
    };
  }

  for (const auto &x : props_config)
  {
    puts("PROP");
    for (const auto &kv : x.values)
    {
      printf("%s: %s\n", kv.first.c_str(), kv.second.c_str());
    }
  }
  puts("WORLD");
  for (const auto &kv : world_config.values)
  {
    printf("%s: %s\n", kv.first.c_str(), kv.second.c_str());
  }
}
