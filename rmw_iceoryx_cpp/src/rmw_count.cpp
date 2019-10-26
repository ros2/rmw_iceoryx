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

#include "rcutils/error_handling.h"
#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

#include "rmw_iceoryx_cpp/iceoryx_topic_names_and_types.hpp"

extern "C"
{
rmw_ret_t
rmw_count_publishers(
  const rmw_node_t * node,
  const char * topic_name,
  size_t * count)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(topic_name, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(count, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_count_publishers
    : node, node->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  auto nodes_and_publishers = rmw_iceoryx_cpp::get_nodes_and_publishers();

  size_t counter = 0;

  for (auto node : nodes_and_publishers) {
    for (auto topic : node.second) {
      if (topic == topic_name) {
        ++counter;
      }
    }
  }

  *count = counter;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_count_subscribers(
  const rmw_node_t * node,
  const char * topic_name,
  size_t * count)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(topic_name, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(count, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_count_subscribers
    : node, node->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  auto nodes_and_subscribers = rmw_iceoryx_cpp::get_nodes_and_subscribers();

  size_t counter = 0;

  for (auto node : nodes_and_subscribers) {
    for (auto topic : node.second) {
      if (topic == topic_name) {
        ++counter;
      }
    }
  }

  *count = counter;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_subscription_count_matched_publishers(
  const rmw_subscription_t * subscription,
  size_t * publisher_count)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(publisher_count, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_subscription_count_matched_publishers
    : subscription, subscription->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  auto nodes_and_publishers = rmw_iceoryx_cpp::get_nodes_and_publishers();

  size_t counter = 0;

  for (auto node : nodes_and_publishers) {
    for (auto topic : node.second) {
      if (topic == subscription->topic_name) {
        ++counter;
      }
    }
  }

  *publisher_count = counter;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_publisher_count_matched_subscriptions(
  const rmw_publisher_t * publisher,
  size_t * subscription_count)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription_count, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_publisher_count_matched_subscriptions
    : publisher, publisher->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  auto nodes_and_subscribers = rmw_iceoryx_cpp::get_nodes_and_subscribers();

  size_t counter = 0;

  for (auto node : nodes_and_subscribers) {
    for (auto topic : node.second) {
      if (topic == publisher->topic_name) {
        ++counter;
      }
    }
  }

  *subscription_count = counter;
  return RMW_RET_OK;
}
}  // extern "C"
