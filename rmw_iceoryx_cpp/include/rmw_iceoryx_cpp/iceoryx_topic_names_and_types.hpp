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

#ifndef RMW_ICEORYX_CPP__ICEORYX_TOPIC_NAMES_AND_TYPES_HPP_
#define RMW_ICEORYX_CPP__ICEORYX_TOPIC_NAMES_AND_TYPES_HPP_

#include <map>
#include <string>
#include <vector>

#include "rcutils/types.h"

#include "rmw/names_and_types.h"
#include "rmw/rmw.h"

namespace rmw_iceoryx_cpp
{

void fill_topic_containers(
  std::map<std::string, std::string> & names_n_types_,
  std::map<std::string, std::vector<std::string>> & subscribers_topics_,
  std::map<std::string, std::vector<std::string>> & publishers_topics_,
  std::map<std::string, std::vector<std::string>> & topic_subscribers_,
  std::map<std::string, std::vector<std::string>> & topic_publishers_);

std::map<std::string, std::string> get_topic_names_and_types();

std::map<std::string, std::vector<std::string>> get_nodes_and_publishers();

std::map<std::string, std::vector<std::string>> get_nodes_and_subscribers();

std::map<std::string, std::string> get_publisher_names_and_types_of_node(
  const char * node_name,
  const char * node_namespace);

std::map<std::string, std::string> get_subscription_names_and_types_of_node(
  const char * node_name,
  const char * node_namespace);

rmw_ret_t fill_rmw_names_and_types(
  rmw_names_and_types_t * rmw_topic_names_and_types,
  const std::map<std::string, std::string> & iceoryx_topic_names_and_types,
  rcutils_allocator_t * allocator);

}  // namespace rmw_iceoryx_cpp
#endif  // RMW_ICEORYX_CPP__ICEORYX_TOPIC_NAMES_AND_TYPES_HPP_
