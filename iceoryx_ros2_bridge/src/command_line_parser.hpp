// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef COMMAND_LINE_PARSER_HPP_
#define COMMAND_LINE_PARSER_HPP_

#include <string>
#include <vector>

class CommandlineParser
{
public:
  CommandlineParser(int argc, char ** argv, std::function<void(void)> usage_cb)
  : argc_(argc), argv_(argv), usage_cb_(usage_cb)
  {}

  size_t size()
  {
    return argc_ - 1;  // first arg is program name
  }

  bool option_exists(const std::string & option)
  {
    return std::find(argv_, argv_ + argc_, option) != argv_ + argc_;
  }

  std::vector<std::string> get_option(const std::string & option)
  {
    std::vector<std::string> result = {};

    auto it = std::find(argv_, argv_ + argc_, option);
    if (it == argv_ + argc_) {
      return result;
    }
    ++it;
    auto end = std::find_if(
      argv_ + std::distance(argv_, it),
      argv_ + argc_,
      [](const char * str) {
        return str[0] == '-';
      });  // it's either the next arg or end
    while (it != end) {
      result.push_back(*it);
      ++it;
    }

    if (result.empty()) {
      fprintf(stderr, "Error: Wrong argument format\n");
      fprintf(stderr, "%s option requires a list of arguments\n", option.c_str());
      usage_cb_();
      exit(-1);
    }
    return result;
  }

private:
  int argc_;
  char ** argv_;
  std::function<void(void)> usage_cb_;
};


#endif  // COMMAND_LINE_PARSER_HPP_
