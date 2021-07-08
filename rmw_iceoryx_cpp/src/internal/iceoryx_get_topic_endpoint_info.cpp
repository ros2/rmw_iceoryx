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

#include <map>
#include <string>
#include <vector>
#include <tuple>

#include "iceoryx_posh/popo/untyped_subscriber.hpp"
#include "iceoryx_posh/roudi/introspection_types.hpp"

#include "rcutils/logging_macros.h"
#include "rcutils/strdup.h"

#include "rmw/impl/cpp/macros.hpp"

#include "rmw_iceoryx_cpp/iceoryx_name_conversion.hpp"
#include "rmw_iceoryx_cpp/iceoryx_topic_names_and_types.hpp"
#include "rmw_iceoryx_cpp/iceoryx_get_topic_endpoint_info.hpp"

namespace rmw_iceoryx_cpp
{
std::map<std::string, std::vector<std::string>> get_publisher_and_nodes()
{
  std::map<std::string, std::string> names_n_types;
  std::map<std::string, std::vector<std::string>> subscribers_topics;
  std::map<std::string, std::vector<std::string>> publishers_topics;
  std::map<std::string, std::vector<std::string>> topic_subscribers;
  std::map<std::string, std::vector<std::string>> topic_publishers;

  fill_topic_containers(
    names_n_types, subscribers_topics, publishers_topics, topic_subscribers,
    topic_publishers);

  return topic_publishers;
}

std::map<std::string, std::vector<std::string>> get_subscriber_and_nodes()
{
  std::map<std::string, std::string> names_n_types;
  std::map<std::string, std::vector<std::string>> subscribers_topics;
  std::map<std::string, std::vector<std::string>> publishers_topics;
  std::map<std::string, std::vector<std::string>> topic_subscribers;
  std::map<std::string, std::vector<std::string>> topic_publishers;

  fill_topic_containers(
    names_n_types, subscribers_topics, publishers_topics, topic_subscribers,
    topic_publishers);

  return topic_subscribers;
}

std::tuple<std::string, std::vector<std::string>> get_publisher_end_info_of_topic(
  const char * topic_name)
{
  std::map<std::string, std::string> names_n_types;
  std::map<std::string, std::vector<std::string>> subscribers_topics;
  std::map<std::string, std::vector<std::string>> publishers_topics;
  std::map<std::string, std::vector<std::string>> topic_subscribers;
  std::map<std::string, std::vector<std::string>> topic_publishers;

  fill_topic_containers(
    names_n_types, subscribers_topics, publishers_topics, topic_subscribers,
    topic_publishers);
  auto full_name_array = topic_publishers[std::string(topic_name)];
  auto topic_type = names_n_types[topic_name];
  return std::make_tuple(topic_type, full_name_array);
}

std::tuple<std::string, std::vector<std::string>> get_subscriber_end_info_of_topic(
  const char * topic_name)
{
  std::map<std::string, std::string> names_n_types;
  std::map<std::string, std::vector<std::string>> subscribers_topics;
  std::map<std::string, std::vector<std::string>> publishers_topics;
  std::map<std::string, std::vector<std::string>> topic_subscribers;
  std::map<std::string, std::vector<std::string>> topic_publishers;

  fill_topic_containers(
    names_n_types, subscribers_topics, publishers_topics, topic_subscribers,
    topic_publishers);
  auto full_name_array = topic_subscribers[std::string(topic_name)];
  auto topic_type = names_n_types[topic_name];
  return std::make_tuple(topic_type, full_name_array);
}

rmw_ret_t
fill_rmw_publisher_end_info(
  rmw_topic_endpoint_info_array_t * rmw_topic_endpoint_info_array,
  const std::tuple<std::string, std::vector<std::string>> & iceoryx_topic_endpoint_info,
  rcutils_allocator_t * allocator)
{
  rmw_ret_t rmw_ret = RMW_RET_ERROR;

  auto topic_type = std::get<0>(iceoryx_topic_endpoint_info);
  auto full_name_array = std::get<1>(iceoryx_topic_endpoint_info);

  if (!full_name_array.empty()) {
    rmw_ret = rmw_topic_endpoint_info_array_init_with_size(
      rmw_topic_endpoint_info_array, full_name_array.size(),
      allocator);
    if (rmw_ret != RMW_RET_OK) {
      return rmw_ret;
    }
  }

  int i = 0;
  // store all data in rmw_topic_endpoint_info_array_t
  for (const auto node_full_name : full_name_array) {
    auto name_n_space = get_name_n_space_from_node_full_name(node_full_name);
    auto rmw_topic_endpoint_info = rmw_get_zero_initialized_topic_endpoint_info();
    // duplicate and store the topic_name
    char * topic_type_cstr = rcutils_strdup(topic_type.c_str(), *allocator);
    if (!topic_type_cstr) {
      RMW_SET_ERROR_MSG("failed to allocate memory for topic_type");
      goto fail;
    }
    rmw_ret = rmw_topic_endpoint_info_set_topic_type(
      &rmw_topic_endpoint_info, topic_type_cstr,
      allocator);
    if (rmw_ret != RMW_RET_OK) {
      RCUTILS_LOG_ERROR("error during report of error: %s", rmw_get_error_string().str);
      goto fail;
    }

    char * node_name = rcutils_strdup(std::get<0>(name_n_space).c_str(), *allocator);
    if (!node_name) {
      RMW_SET_ERROR_MSG("failed to allocate memory for node_name");
      goto fail;
    }
    rmw_ret = rmw_topic_endpoint_info_set_node_name(&rmw_topic_endpoint_info, node_name, allocator);
    if (rmw_ret != RMW_RET_OK) {
      RCUTILS_LOG_ERROR("error during report of error: %s", rmw_get_error_string().str);
      goto fail;
    }

    char * node_namespace = rcutils_strdup(std::get<1>(name_n_space).c_str(), *allocator);
    if (!node_namespace) {
      RMW_SET_ERROR_MSG("failed to allocate memory for node_namespace");
      goto fail;
    }
    rmw_ret = rmw_topic_endpoint_info_set_node_namespace(
      &rmw_topic_endpoint_info, node_namespace,
      allocator);
    if (rmw_ret != RMW_RET_OK) {
      RCUTILS_LOG_ERROR("error during report of error: %s", rmw_get_error_string().str);
      goto fail;
    }

    rmw_ret = rmw_topic_endpoint_info_set_endpoint_type(
      &rmw_topic_endpoint_info,
      RMW_ENDPOINT_PUBLISHER);
    if (rmw_ret != RMW_RET_OK) {
      RCUTILS_LOG_ERROR("error during report of error: %s", rmw_get_error_string().str);
      goto fail;
    }
    /// @todo support gid and qos setting

    // set array
    rmw_topic_endpoint_info_array->info_array[i] = rmw_topic_endpoint_info;
    i++;
  }

  return RMW_RET_OK;

fail:
  rmw_ret = rmw_topic_endpoint_info_array_fini(rmw_topic_endpoint_info_array, allocator);
  if (rmw_ret != RMW_RET_OK) {
    RCUTILS_LOG_ERROR("error during report of error: %s", rmw_get_error_string().str);
  }
  return RMW_RET_ERROR;
}

rmw_ret_t
fill_rmw_subscriber_end_info(
  rmw_topic_endpoint_info_array_t * rmw_topic_endpoint_info_array,
  const std::tuple<std::string, std::vector<std::string>> & iceoryx_topic_endpoint_info,
  rcutils_allocator_t * allocator)
{
  rmw_ret_t rmw_ret = RMW_RET_ERROR;

  auto topic_type = std::get<0>(iceoryx_topic_endpoint_info);
  auto full_name_array = std::get<1>(iceoryx_topic_endpoint_info);

  if (!full_name_array.empty()) {
    rmw_ret = rmw_topic_endpoint_info_array_init_with_size(
      rmw_topic_endpoint_info_array, full_name_array.size(),
      allocator);
    if (rmw_ret != RMW_RET_OK) {
      return rmw_ret;
    }
  }

  int i = 0;
  // store all data in rmw_topic_endpoint_info_array_t
  for (const auto node_full_name : full_name_array) {
    auto name_n_space = get_name_n_space_from_node_full_name(node_full_name);
    auto rmw_topic_endpoint_info = rmw_get_zero_initialized_topic_endpoint_info();
    // duplicate and store the topic_name
    char * topic_type_cstr = rcutils_strdup(topic_type.c_str(), *allocator);
    if (!topic_type_cstr) {
      RMW_SET_ERROR_MSG("failed to allocate memory for topic_type");
      goto fail;
    }
    rmw_ret = rmw_topic_endpoint_info_set_topic_type(
      &rmw_topic_endpoint_info, topic_type_cstr,
      allocator);
    if (rmw_ret != RMW_RET_OK) {
      RCUTILS_LOG_ERROR("error during report of error: %s", rmw_get_error_string().str);
      goto fail;
    }

    char * node_name = rcutils_strdup(std::get<0>(name_n_space).c_str(), *allocator);
    if (!node_name) {
      RMW_SET_ERROR_MSG("failed to allocate memory for node_name");
      goto fail;
    }
    rmw_ret = rmw_topic_endpoint_info_set_node_name(&rmw_topic_endpoint_info, node_name, allocator);
    if (rmw_ret != RMW_RET_OK) {
      RCUTILS_LOG_ERROR("error during report of error: %s", rmw_get_error_string().str);
      goto fail;
    }

    char * node_namespace = rcutils_strdup(std::get<1>(name_n_space).c_str(), *allocator);
    if (!node_namespace) {
      RMW_SET_ERROR_MSG("failed to allocate memory for node_namespace");
      goto fail;
    }
    rmw_ret = rmw_topic_endpoint_info_set_node_namespace(
      &rmw_topic_endpoint_info, node_namespace,
      allocator);
    if (rmw_ret != RMW_RET_OK) {
      RCUTILS_LOG_ERROR("error during report of error: %s", rmw_get_error_string().str);
      goto fail;
    }

    rmw_ret = rmw_topic_endpoint_info_set_endpoint_type(
      &rmw_topic_endpoint_info,
      RMW_ENDPOINT_SUBSCRIPTION);
    if (rmw_ret != RMW_RET_OK) {
      RCUTILS_LOG_ERROR("error during report of error: %s", rmw_get_error_string().str);
      goto fail;
    }
    /// @todo support gid and qos setting

    // set array
    rmw_topic_endpoint_info_array->info_array[i] = rmw_topic_endpoint_info;
    i++;
  }

  return RMW_RET_OK;

fail:
  rmw_ret = rmw_topic_endpoint_info_array_fini(rmw_topic_endpoint_info_array, allocator);
  if (rmw_ret != RMW_RET_OK) {
    RCUTILS_LOG_ERROR("error during report of error: %s", rmw_get_error_string().str);
  }
  return RMW_RET_ERROR;
}

}  // namespace rmw_iceoryx_cpp
