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

#include <map>
#include <string>
#include <vector>

#include "iceoryx_posh/popo/subscriber.hpp"
#include "iceoryx_posh/roudi/introspection_types.hpp"

#include "rcutils/logging_macros.h"
#include "rcutils/strdup.h"

#include "rmw/impl/cpp/macros.hpp"

#include "rmw_iceoryx_cpp/iceoryx_name_conversion.hpp"
#include "rmw_iceoryx_cpp/iceoryx_topic_names_and_types.hpp"

namespace rmw_iceoryx_cpp
{

// TODO(mphnl) refactoring, too many copies
void fill_topic_containers(
  std::map<std::string, std::string> & names_n_types_,
  std::map<std::string, std::vector<std::string>> & subscribers_topics_,
  std::map<std::string, std::vector<std::string>> & publishers_topics_)
{
  static iox::popo::Subscriber port_receiver(iox::roudi::IntrospectionPortService);
  static std::map<std::string, std::string> names_n_types;
  static std::map<std::string, std::vector<std::string>> subscribers_topics;
  static std::map<std::string, std::vector<std::string>> publishers_topics;

  bool updated = false;
  if (iox::popo::SubscriptionState::SUBSCRIBED != port_receiver.getSubscriptionState()) {
    port_receiver.subscribe(1);
    // wait for delivery on subscribe
    while (!port_receiver.hasNewChunks()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    updated = true;
  } else {
    updated = port_receiver.hasNewChunks();
  }

  if (updated) {
    // get the latest sample
    const iox::mepoo::ChunkHeader * chunk_header = nullptr;
    const iox::mepoo::ChunkHeader * latest_chunk_header = nullptr;

    while (port_receiver.getChunk(&chunk_header)) {
      if (latest_chunk_header) {
        port_receiver.releaseChunk(latest_chunk_header);
      }
      latest_chunk_header = chunk_header;
    }

    if (latest_chunk_header) {
      const iox::roudi::PortIntrospectionFieldTopic * port_sample =
        static_cast<const iox::roudi::PortIntrospectionFieldTopic *>(latest_chunk_header->
        m_payload);

      names_n_types.clear();
      subscribers_topics.clear();
      publishers_topics.clear();

      for (auto & receiver : port_sample->m_receiverList) {
        auto name_and_type = rmw_iceoryx_cpp::get_name_n_type_from_iceoryx_service_description(
          std::string(receiver.m_caproServiceID.to_cstring()),
          std::string(receiver.m_caproInstanceID.to_cstring()),
          std::string(receiver.m_caproEventMethodID.to_cstring()));

        names_n_types[std::get<0>(name_and_type)] = std::get<1>(name_and_type);
        subscribers_topics[std::string(receiver.m_runnable.to_cstring())].push_back(std::get<0>(
            name_and_type));
      }
      for (auto & sender : port_sample->m_senderList) {
        auto name_and_type = rmw_iceoryx_cpp::get_name_n_type_from_iceoryx_service_description(
          std::string(sender.m_caproServiceID.to_cstring()),
          std::string(sender.m_caproInstanceID.to_cstring()),
          std::string(sender.m_caproEventMethodID.to_cstring()));

        names_n_types[std::get<0>(name_and_type)] = std::get<1>(name_and_type);
        publishers_topics[std::string(sender.m_runnable.to_cstring())].push_back(std::get<0>(
            name_and_type));
      }
      port_receiver.releaseChunk(latest_chunk_header);
    }
  }

  names_n_types_ = names_n_types;
  subscribers_topics_ = subscribers_topics;
  publishers_topics_ = publishers_topics;
}

std::map<std::string, std::string> get_topic_names_and_types()
{
  std::map<std::string, std::string> names_n_types;
  std::map<std::string, std::vector<std::string>> subscribers_topics;
  std::map<std::string, std::vector<std::string>> publishers_topics;

  fill_topic_containers(names_n_types, subscribers_topics, publishers_topics);

  return names_n_types;
}

std::map<std::string, std::vector<std::string>> get_nodes_and_publishers()
{
  std::map<std::string, std::string> names_n_types;
  std::map<std::string, std::vector<std::string>> subscribers_topics;
  std::map<std::string, std::vector<std::string>> publishers_topics;

  fill_topic_containers(names_n_types, subscribers_topics, publishers_topics);

  return publishers_topics;
}

std::map<std::string, std::vector<std::string>> get_nodes_and_subscribers()
{
  std::map<std::string, std::string> names_n_types;
  std::map<std::string, std::vector<std::string>> subscribers_topics;
  std::map<std::string, std::vector<std::string>> publishers_topics;

  fill_topic_containers(names_n_types, subscribers_topics, publishers_topics);

  return subscribers_topics;
}

std::map<std::string, std::string> get_publisher_names_and_types_of_node(
  const char * node_name,
  const char * node_namespace)
{
  std::map<std::string, std::string> names_n_types;
  std::map<std::string, std::vector<std::string>> subscribers_topics;
  std::map<std::string, std::vector<std::string>> publishers_topics;

  fill_topic_containers(names_n_types, subscribers_topics, publishers_topics);

  std::map<std::string, std::string> publisher_names_and_types;

  std::string full_name = std::string(node_namespace) + std::string(node_name);

  auto publisher_topics = publishers_topics[full_name];
  for (auto topic : publisher_topics) {
    publisher_names_and_types[topic] = names_n_types[topic];
  }
  return publisher_names_and_types;
}

std::map<std::string, std::string> get_subscription_names_and_types_of_node(
  const char * node_name,
  const char * node_namespace)
{
  std::map<std::string, std::string> names_n_types;
  std::map<std::string, std::vector<std::string>> subscribers_topics;
  std::map<std::string, std::vector<std::string>> publishers_topics;

  fill_topic_containers(names_n_types, subscribers_topics, publishers_topics);

  std::map<std::string, std::string> subscriber_names_and_types;

  std::string full_name = std::string(node_namespace) + std::string(node_name);

  auto subscriber_topics = subscribers_topics[full_name];
  for (auto topic : subscriber_topics) {
    subscriber_names_and_types[topic] = names_n_types[topic];
  }
  return subscriber_names_and_types;
}

rmw_ret_t fill_rmw_names_and_types(
  rmw_names_and_types_t * rmw_topic_names_and_types,
  const std::map<std::string, std::string> & iceoryx_topic_names_and_types,
  rcutils_allocator_t * allocator)
{
  rmw_ret_t rmw_ret = RMW_RET_ERROR;

  if (!iceoryx_topic_names_and_types.empty()) {
    rmw_ret = rmw_names_and_types_init(rmw_topic_names_and_types,
        iceoryx_topic_names_and_types.size(), allocator);
    if (rmw_ret != RMW_RET_OK) {
      return rmw_ret;
    }

    // store all topic names and their types in rmw_names_and_types_t
    size_t index = 0;
    for (const auto & name_n_type : iceoryx_topic_names_and_types) {
      // duplicate and store the topic_name
      char * topic_name = rcutils_strdup(name_n_type.first.c_str(), *allocator);
      if (!topic_name) {
        RMW_SET_ERROR_MSG("failed to allocate memory for topic name");
        goto fail;
      }
      rmw_topic_names_and_types->names.data[index] = topic_name;

      // initialize the string array for types
      {
        rcutils_ret_t rcutils_ret = rcutils_string_array_init(
          &rmw_topic_names_and_types->types[index],
          1,  // one type only in iceoryx
          allocator);
        if (rcutils_ret != RCUTILS_RET_OK) {
          RMW_SET_ERROR_MSG(rcutils_get_error_string().str);
          goto fail;
        }
      }
      // duplicate and store the topic type
      char * type_name = rcutils_strdup(name_n_type.second.c_str(), *allocator);
      if (!type_name) {
        RMW_SET_ERROR_MSG("failed to allocate memory for type name");
        goto fail;
      }
      rmw_topic_names_and_types->types[index].data[0] = type_name;
      ++index;
    }
  }

  return RMW_RET_OK;

fail:
  rmw_ret = rmw_names_and_types_fini(rmw_topic_names_and_types);
  if (rmw_ret != RMW_RET_OK) {
    RCUTILS_LOG_ERROR("error during report of error: %s", rmw_get_error_string().str);
  }
  return RMW_RET_ERROR;
}

}  // namespace rmw_iceoryx_cpp
