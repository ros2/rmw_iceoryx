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

#ifndef RMW_ICEORYX_CPP__ICEORYX_GET_TOPIC_ENDPOINT_INFO_HPP_
#define RMW_ICEORYX_CPP__ICEORYX_GET_TOPIC_ENDPOINT_INFO_HPP_

#include <map>
#include <string>
#include <vector>
#include <tuple>

#include "rcutils/types.h"

#include "rmw/names_and_types.h"
#include "rmw/rmw.h"
#include "rmw/topic_endpoint_info_array.h"

namespace rmw_iceoryx_cpp
{
/**
 * \brief    Get topics and their publisher nodes
 * \return   std::map<std::string, std::vector<std::string>>
 */
std::map<std::string, std::vector<std::string>> get_publisher_and_nodes();

/**
 * \brief    Get topics and their subscriber nodes
 * \return   std::map<std::string, std::vector<std::string>>
 */
std::map<std::string, std::vector<std::string>> get_subscriber_and_nodes();

/**
 * \brief    Get publisher node names of one topic
 * \param    topic_name
 * \return   std::tuple<std::string, std::vector<std::string>>
 */
std::tuple<std::string, std::vector<std::string>> get_publisher_end_info_of_topic(
  const char * topic_name);

/**
 * \brief    Get  subscriber node names of one topic
 * \param    topic_name
 * \return   std::tuple<std::string, std::vector<std::string>>
 */
std::tuple<std::string, std::vector<std::string>> get_subscriber_end_info_of_topic(
  const char * topic_name);

/**
 * \brief     helper function to fill rmw structs
 * \param    rmw_topic_endpoint_info_array
 * \param    iceoryx_topic_endpoint_info_array
 * \param    allocator
 * \return   rmw_ret_t
 */
rmw_ret_t
fill_rmw_publisher_end_info(
  rmw_topic_endpoint_info_array_t * rmw_topic_endpoint_info_array,
  const std::tuple<std::string, std::vector<std::string>> & iceoryx_topic_endpoint_info_array,
  rcutils_allocator_t * allocator);

/**
 * \brief     helper function to fill rmw structs
 * \param    rmw_topic_endpoint_info_array
 * \param    iceoryx_topic_endpoint_info_array
 * \param    allocator
 * \return   rmw_ret_t
 */
rmw_ret_t
fill_rmw_subscriber_end_info(
  rmw_topic_endpoint_info_array_t * rmw_topic_endpoint_info_array,
  const std::tuple<std::string, std::vector<std::string>> & iceoryx_topic_endpoint_info_array,
  rcutils_allocator_t * allocator);

}  // namespace rmw_iceoryx_cpp
#endif  // RMW_ICEORYX_CPP__ICEORYX_GET_TOPIC_ENDPOINT_INFO_HPP_
