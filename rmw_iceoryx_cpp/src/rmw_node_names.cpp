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

#include <set>
#include <string>

#include "iceoryx_posh/popo/subscriber.hpp"
#include "iceoryx_posh/roudi/introspection_types.hpp"

#include "rcutils/error_handling.h"
#include "rcutils/logging_macros.h"
#include "rcutils/strdup.h"

#include "rmw/convert_rcutils_ret_to_rmw_ret.h"
#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

extern "C"
{
rmw_ret_t
rmw_get_node_names(
  const rmw_node_t * node,
  rcutils_string_array_t * node_names,
  rcutils_string_array_t * node_namespaces)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node_names, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node_namespaces, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_get_node_names
    : node, node->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  static iox::popo::Subscriber process_receiver(iox::roudi::IntrospectionProcessService);
  static std::set<std::string> node_names_set;

  bool updated = false;
  if (iox::popo::SubscriptionState::SUBSCRIBED != process_receiver.getSubscriptionState()) {
    process_receiver.subscribe(1);
    // wait for delivery on subscribe
    while (!process_receiver.hasNewChunks()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    updated = true;
  } else {
    updated = process_receiver.hasNewChunks();
  }

  if (updated) {
    // get the latest sample
    const iox::mepoo::ChunkHeader * chunk_header = nullptr;
    const iox::mepoo::ChunkHeader * latest_chunk_header = nullptr;

    while (process_receiver.getChunk(&chunk_header)) {
      if (latest_chunk_header) {
        process_receiver.releaseChunk(latest_chunk_header);
      }
      latest_chunk_header = chunk_header;
    }

    if (latest_chunk_header) {
      const iox::roudi::ProcessIntrospectionFieldTopic * process_sample =
        static_cast<const iox::roudi::ProcessIntrospectionFieldTopic *>(latest_chunk_header->
        m_payload);

      node_names_set.clear();
      for (auto & process : process_sample->m_processList) {
        for (auto & runnable : process.m_runnables) {
          node_names_set.insert(std::string(runnable.to_cstring()));
        }
      }
      process_receiver.releaseChunk(latest_chunk_header);
    }
  }

  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  rcutils_ret_t rcutils_ret =
    rcutils_string_array_init(node_names, node_names_set.size(), &allocator);
  if (rcutils_ret != RCUTILS_RET_OK) {
    RMW_SET_ERROR_MSG(rcutils_get_error_string().str);
    return rmw_convert_rcutils_ret_to_rmw_ret(rcutils_ret);
  }

  rcutils_ret = rcutils_string_array_init(node_namespaces, node_names_set.size(), &allocator);
  if (rcutils_ret != RCUTILS_RET_OK) {
    RMW_SET_ERROR_MSG(rcutils_get_error_string().str);
    return rmw_convert_rcutils_ret_to_rmw_ret(rcutils_ret);
  }

  int i = 0;
  for (auto name : node_names_set) {
    auto pos = name.rfind("/");

    auto node_name = name.substr(pos + 1, name.size());
    auto node_namespace = name.substr(0, pos);

    node_names->data[i] = rcutils_strdup(node_name.c_str(), allocator);
    if (!node_names->data[i]) {
      RMW_SET_ERROR_MSG("could not allocate memory for node name");
      goto fail;
    }

    node_namespaces->data[i] = rcutils_strdup(node_namespace.c_str(), allocator);
    if (!node_namespaces->data[i]) {
      RMW_SET_ERROR_MSG("could not allocate memory for node namespace");
      goto fail;
    }
    ++i;
  }

  return RMW_RET_OK;

fail:
  if (node_names) {
    rcutils_ret = rcutils_string_array_fini(node_names);
    if (rcutils_ret != RCUTILS_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        "rmw_connext_cpp",
        "failed to cleanup during error handling: %s", rcutils_get_error_string().str);
      rcutils_reset_error();
    }
  }
  if (node_namespaces) {
    rcutils_ret = rcutils_string_array_fini(node_namespaces);
    if (rcutils_ret != RCUTILS_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        "rmw_connext_cpp",
        "failed to cleanup during error handling: %s", rcutils_get_error_string().str);
      rcutils_reset_error();
    }
  }
  return RMW_RET_BAD_ALLOC;
}
}  // extern "C"
