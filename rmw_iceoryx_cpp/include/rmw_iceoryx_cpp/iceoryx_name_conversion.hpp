// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2021 by ZhenshengLee. All rights reserved.
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

#ifndef RMW_ICEORYX_CPP__ICEORYX_NAME_CONVERSION_HPP_
#define RMW_ICEORYX_CPP__ICEORYX_NAME_CONVERSION_HPP_

#include <string>
#include <tuple>

#include "iceoryx_posh/capro/service_description.hpp"
#include "iceoryx_utils/cxx/string.hpp"

struct rosidl_message_type_support_t;

namespace rmw_iceoryx_cpp
{
/// Get the pair of ros node name and namespace from a given iceoryx node full name.
/**
 * \brief    Get the name n space from node full name object
 * \param    node_full_name full name of the node
 * \return   std::tuple<std::string, std::string>
 */
std::tuple<std::string, std::string>
get_name_n_space_from_node_full_name(
  const std::string & node_full_name);

/// Get the pair of ROS topic and type from a given iceoryx service triplet.
/**
 * Given a triplet of `service`, `instance`, `event`, convert these to a ROS2 conform
 * tuple of `topic` and `type`.
 * For a regular ROS2 pair, the event name should be set to "data".
 * \param service the iceoryx service description
 * \param instance the iceoryx instance description
 * \param event the iceoryx event description
 * \return tuple of topic and type
 */
std::tuple<std::string, std::string>
get_name_n_type_from_service_description(
  const std::string & service,
  const std::string & instance,
  const std::string & event);

/// Get the iceoryx service triplet description from a given pair of ROS topic and type.
/**
 * Given a pair in the form of topic name and type, generate a iceoryx service description triplet.
 * By convention, the iceoryx event field is set to "data".
 * \param service the iceoryx service description
 * \param instance the iceoryx instance description
 * \param event the iceoryx event description
 * \return triple of iceoryx `service`, `instance`, `event`.
 */
std::tuple<std::string, std::string, std::string>
get_service_description_from_name_n_type(
  const std::string & topic_name,
  const std::string & type_name);

iox::capro::ServiceDescription
get_iceoryx_service_description(
  const std::string & topic,
  const rosidl_message_type_support_t * type_supports);

}  // namespace rmw_iceoryx_cpp
#endif  // RMW_ICEORYX_CPP__ICEORYX_NAME_CONVERSION_HPP_
