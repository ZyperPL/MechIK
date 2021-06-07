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

  std::string key {}, value {};
  
  world_config = std::make_shared<ConfigKeysValues>();

  auto add_key_value = [&section, this](const std::string key, const std::string value) {
    if (section == World)
      world_config->insert({ key, value });
    else if (section == Prop)
      props_config.back()->insert({ key, value });
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
            props_config.push_back(std::make_shared<ConfigKeysValues>());
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

#ifdef PRINT_CONFIG_VALUES
  for (const auto &x : props_config)
  {
    puts("PROP");
    for (const auto &kv : x)
    {
      printf("%s: %s\n", kv.first.c_str(), kv.second.c_str());
    }
  }
  puts("WORLD");
  for (const auto &kv : world_config)
  {
    printf("%s: %s\n", kv.first.c_str(), kv.second.c_str());
  }
#endif
}
