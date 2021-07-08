// Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
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

#include "rcutils/error_handling.h"
#include "rmw/get_topic_endpoint_info.h"
#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"
#include "rmw/topic_endpoint_info_array.h"

#include "rmw_iceoryx_cpp/iceoryx_topic_names_and_types.hpp"
#include "rmw_iceoryx_cpp/iceoryx_get_topic_endpoint_info.hpp"

extern "C" {
rmw_ret_t rmw_get_publishers_info_by_topic(
  const rmw_node_t * node, rcutils_allocator_t * allocator,
  const char * topic_name, bool no_mangle,
  rmw_topic_endpoint_info_array_t * publishers_info)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(allocator, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(topic_name, RMW_RET_ERROR);
  (void)no_mangle;

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_get_topic_names_and_types
    : node, node->implementation_identifier, rmw_get_implementation_identifier(),
    return RMW_RET_ERROR);

  rmw_ret_t rmw_ret = rmw_topic_endpoint_info_array_check_zero(publishers_info);
  if (rmw_ret != RMW_RET_OK) {
    return rmw_ret;      // error already set
  }

  auto iceoryx_publisher_endpoint_info =
    rmw_iceoryx_cpp::get_publisher_end_info_of_topic(topic_name);

  return rmw_iceoryx_cpp::fill_rmw_publisher_end_info(
    publishers_info,
    iceoryx_publisher_endpoint_info, allocator);
}

rmw_ret_t rmw_get_subscriptions_info_by_topic(
  const rmw_node_t * node, rcutils_allocator_t * allocator,
  const char * topic_name, bool no_mangle,
  rmw_topic_endpoint_info_array_t * subscriptions_info)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(allocator, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(topic_name, RMW_RET_ERROR);
  (void)no_mangle;

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_get_topic_names_and_types
    : node, node->implementation_identifier, rmw_get_implementation_identifier(),
    return RMW_RET_ERROR);

  rmw_ret_t rmw_ret = rmw_topic_endpoint_info_array_check_zero(subscriptions_info);
  if (rmw_ret != RMW_RET_OK) {
    return rmw_ret;      // error already set
  }

  auto iceoryx_subscriber_endpoint_info = rmw_iceoryx_cpp::get_subscriber_end_info_of_topic(
    topic_name);

  return rmw_iceoryx_cpp::fill_rmw_subscriber_end_info(
    subscriptions_info, iceoryx_subscriber_endpoint_info,
    allocator);
}
}  // extern "C"
