// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2021 by Apex.AI Inc. All rights reserved.
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

#include <string>

#include "iceoryx_posh/runtime/node.hpp"

#include "rmw/allocators.h"

#include "./types/iceoryx_node.hpp"

extern "C"
{
rmw_node_t *
rmw_create_node(
  rmw_context_t * context,
  const char * name,
  const char * namespace_)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(context, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(name, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(namespace_, nullptr);


  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_create_node
    : context, context->implementation_identifier,
    rmw_get_implementation_identifier(), return nullptr);

  std::string full_name = std::string(namespace_) + std::string(name);
  rmw_guard_condition_t * guard_condition = nullptr;
  IceoryxGraphChangeNotifier * graph_change_notifier = nullptr;
  iox::runtime::Node * iceoryx_runnable = nullptr;
  IceoryxNodeInfo * node_info = nullptr;

  rmw_node_t * node_handle = rmw_node_allocate();
  if (!node_handle) {
    RMW_SET_ERROR_MSG("failed to allocate memory for node handle");
    goto fail;
  }

  node_handle->implementation_identifier = rmw_get_implementation_identifier();

  guard_condition = rmw_create_guard_condition(context);
  if (!guard_condition) {
    RMW_SET_ERROR_MSG("failed to create guard condition");
    goto fail;
  }

  /// @todo only use one GraphChangeNotifier for all nodes
  graph_change_notifier =
    static_cast<IceoryxGraphChangeNotifier *>(rmw_allocate(sizeof(IceoryxGraphChangeNotifier)));
  if (!graph_change_notifier) {
    RMW_SET_ERROR_MSG("failed to allocate memory for graph change notifier");
    goto fail;
  }
  RMW_TRY_PLACEMENT_NEW(
    graph_change_notifier, graph_change_notifier, goto fail,
    IceoryxGraphChangeNotifier, guard_condition)

  // allocate iceoryx_runnable
  iceoryx_runnable =
    static_cast<iox::runtime::Node *>(rmw_allocate(
      sizeof(iox::runtime::Node)));
  if (!iceoryx_runnable) {
    RMW_SET_ERROR_MSG("failed to allocate memory for iceoryx runnable");
    goto fail;
  }
  RMW_TRY_PLACEMENT_NEW(
    iceoryx_runnable, iceoryx_runnable,
    goto fail, iox::runtime::Node,
    iox::NodeName_t(iox::cxx::TruncateToCapacity, full_name));

  node_info = static_cast<IceoryxNodeInfo *>(rmw_allocate(sizeof(IceoryxNodeInfo)));
  if (!node_info) {
    RMW_SET_ERROR_MSG("failed to allocate memory for node info");
    goto fail;
  }
  RMW_TRY_PLACEMENT_NEW(
    node_info, node_info, goto fail, IceoryxNodeInfo, guard_condition,
    graph_change_notifier, iceoryx_runnable)

  node_handle->data = node_info;

  node_handle->name = static_cast<const char *>(rmw_allocate(sizeof(char) * strlen(name) + 1));
  if (!node_handle->name) {
    RMW_SET_ERROR_MSG("failed to allocate memory for node name");
    goto fail;
  }
  memcpy(const_cast<char *>(node_handle->name), name, strlen(name) + 1);

  node_handle->namespace_ =
    static_cast<const char *>(rmw_allocate(sizeof(char) * strlen(namespace_) + 1));
  if (!node_handle->namespace_) {
    RMW_SET_ERROR_MSG("failed to allocate memory for node namespace");
    goto fail;
  }
  memcpy(const_cast<char *>(node_handle->namespace_), namespace_, strlen(namespace_) + 1);

  return node_handle;

fail:
  if (node_handle) {
    if (graph_change_notifier) {
      RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(
        graph_change_notifier->~IceoryxGraphChangeNotifier(), IceoryxGraphChangeNotifier)
      rmw_free(graph_change_notifier);
    }
    if (guard_condition) {
      if (RMW_RET_OK != rmw_destroy_guard_condition(guard_condition)) {
        RMW_SET_ERROR_MSG("failed to delete graph guard condition");
      }
    }
    if (iceoryx_runnable) {
      RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(
        iceoryx_runnable->~Node(), iox::runtime::Node)
      rmw_free(iceoryx_runnable);
    }
    if (node_info) {
      rmw_free(node_info);
    }
    if (node_handle->name) {
      rmw_free(const_cast<char *>(node_handle->name));
    }
    if (node_handle->namespace_) {
      rmw_free(const_cast<char *>(node_handle->namespace_));
    }
    rmw_node_free(node_handle);
  }
  return nullptr;
}

rmw_ret_t
rmw_destroy_node(rmw_node_t * node)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_destroy_node
    : node, node->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  rmw_ret_t result = RMW_RET_OK;

  IceoryxNodeInfo * node_info = static_cast<IceoryxNodeInfo *>(node->data);
  if (node_info) {
    if (node_info->graph_change_notifier_) {
      RMW_TRY_DESTRUCTOR(
        node_info->graph_change_notifier_->~IceoryxGraphChangeNotifier(),
        node_info->graph_change_notifier_,
        result = RMW_RET_ERROR)
      rmw_free(node_info->graph_change_notifier_);
    }
    if (RMW_RET_OK != rmw_destroy_guard_condition(node_info->guard_condition_)) {
      RMW_SET_ERROR_MSG("failed to delete graph guard condition");
      result = RMW_RET_ERROR;
    }
    if (node_info->iceoryx_runnable_) {
      RMW_TRY_DESTRUCTOR(
        node_info->iceoryx_runnable_->~Node(),
        node_info->iceoryx_runnable_,
        result = RMW_RET_ERROR)
      rmw_free(node_info->iceoryx_runnable_);
    }
    RMW_TRY_DESTRUCTOR(
      node_info->~IceoryxNodeInfo(),
      node_info_,
      result = RMW_RET_ERROR)
    rmw_free(node_info);
  }
  node->data = nullptr;

  rmw_free(const_cast<char *>(node->name));
  node->name = nullptr;

  rmw_free(const_cast<char *>(node->namespace_));
  node->namespace_ = nullptr;

  rmw_node_free(node);

  return result;
}

rmw_ret_t
rmw_node_assert_liveliness(const rmw_node_t * node)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_node_assert_liveliness
    : node, node->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  /// @todo poehnl: Currently the heartbeat is used that every registered process in iceoryx sends
  /// later this should be extended to a user triggered liveliness that can be send for a runnable

  return RMW_RET_OK;
}

const rmw_guard_condition_t *
rmw_node_get_graph_guard_condition(const rmw_node_t * node)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node->data, nullptr);

  IceoryxNodeInfo * node_info = static_cast<IceoryxNodeInfo *>(node->data);

  return node_info->guard_condition_;
}
}  // extern "C"
