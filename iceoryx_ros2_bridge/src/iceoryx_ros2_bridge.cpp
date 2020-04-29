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

#include <memory>
#include <string>
#include <vector>

#include "iceoryx_posh/popo/publisher.hpp"
#include "iceoryx_posh/popo/subscriber.hpp"
#include "iceoryx_posh/runtime/posh_runtime.hpp"

#include "rclcpp/rclcpp.hpp"
#include "rclcpp/serialization.hpp"
#include "rclcpp/serialized_message.hpp"

#include "rosidl_typesupport_cpp/message_type_support.hpp"

#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"

#include "rmw_iceoryx_cpp/iceoryx_deserialize.hpp"
#include "rmw_iceoryx_cpp/iceoryx_name_conversion.hpp"
#include "rmw_iceoryx_cpp/iceoryx_serialize.hpp"
#include "rmw_iceoryx_cpp/iceoryx_topic_names_and_types.hpp"
#include "rmw_iceoryx_cpp/iceoryx_type_info_introspection.hpp"

#include "./command_line_parser.hpp"
#include "./generic_subscription.hpp"
#include "./generic_publisher.hpp"
#include "./typesupport_helpers.hpp"

using namespace std::chrono_literals;

bool deserialize_into(
  rclcpp::SerializedMessage * serialized_msg,
  const rosidl_message_type_support_t * ts,
  void * ros_msg)
{
  rclcpp::SerializationBase serializer(ts);
  try {
    serializer.deserialize_message(serialized_msg, ros_msg);
  } catch (const std::exception & e) {
    fprintf(stderr, "failed to deserialize: %s\n", e.what());
    return false;
  }

  return true;
}

void publish_to_iceoryx(
  std::shared_ptr<rclcpp::SerializedMessage> serialized_msg,
  const rosidl_message_type_support_t * ts,
  std::shared_ptr<iox::popo::Publisher> iceoryx_publisher)
{
  auto introspection_ts = static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(
    get_message_typesupport_handle(
      ts, rosidl_typesupport_introspection_cpp::typesupport_identifier)->data);

  if (rmw_iceoryx_cpp::iceoryx_is_fixed_size(ts)) {
    void * ros_msg = iceoryx_publisher->allocateChunk(introspection_ts->size_of_);
    introspection_ts->init_function(ros_msg, rosidl_runtime_cpp::MessageInitialization::ALL);

    if (false == deserialize_into(serialized_msg.get(), ts, ros_msg)) {
      iceoryx_publisher->freeChunk(ros_msg);
      return;
    }
    iceoryx_publisher->sendChunk(ros_msg);
  } else {
    void * ros_msg = malloc(introspection_ts->size_of_);
    introspection_ts->init_function(ros_msg, rosidl_runtime_cpp::MessageInitialization::ALL);
    if (false == deserialize_into(serialized_msg.get(), ts, ros_msg)) {
      free(ros_msg);
      return;
    }

    std::vector<char> payload_vector{};
    rmw_iceoryx_cpp::serialize(
      ros_msg,
      introspection_ts,
      payload_vector);
    free(ros_msg);

    void * chunk = iceoryx_publisher->allocateChunk(payload_vector.size(), true);
    memcpy(chunk, payload_vector.data(), payload_vector.size());
    iceoryx_publisher->sendChunk(chunk);
  }
}

bool serialize_into(
  const void * ros_msg,
  const rosidl_message_type_support_t * ts,
  rclcpp::SerializedMessage * serialized_msg)
{
  rclcpp::SerializationBase serializer(ts);
  try {
    serializer.serialize_message(ros_msg, serialized_msg);
  } catch (const std::exception & e) {
    fprintf(stderr, "failed to serialize ros message %s\n", e.what());
    return false;
  }

  return true;
}

void publish_to_ros2(
  std::shared_ptr<iox::popo::Subscriber> subscriber,
  const rosidl_message_type_support_t * ts,
  std::shared_ptr<iceoryx_ros2_bridge::GenericPublisher> ros2_publisher)
{
  rclcpp::SerializedMessage serialized_msg;

  const void * chunk = nullptr;
  subscriber->getChunk(&chunk);

  // ROS2 message is now in chunk. Convert to ROS2 message and then to serialized_message
  if (rmw_iceoryx_cpp::iceoryx_is_fixed_size(ts)) {
    if (false == serialize_into(chunk, ts, &serialized_msg)) {
      subscriber->releaseChunk(chunk);
      return;
    }
  } else {
    // convert chunk to valid ros2 message
    auto introspection_ts =
      static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(
      get_message_typesupport_handle(
        ts, rosidl_typesupport_introspection_cpp::typesupport_identifier)->data);

    void * ros_msg = malloc(introspection_ts->size_of_);
    introspection_ts->init_function(ros_msg, rosidl_runtime_cpp::MessageInitialization::ALL);
    rmw_iceoryx_cpp::deserialize(
      static_cast<const char *>(chunk),
      introspection_ts,
      ros_msg);

    if (false == serialize_into(ros_msg, ts, &serialized_msg)) {
      subscriber->releaseChunk(chunk);
      return;
    }
    introspection_ts->fini_function(ros_msg);
    free(ros_msg);
  }

  fprintf(stderr, "publishing message\n");
  ros2_publisher->publish(&serialized_msg.get_rcl_serialized_message());
  subscriber->releaseChunk(chunk);
}

void usage()
{
  fprintf(stderr, "Usage for iceoryx ros2 bridge\n");
  fprintf(stderr, "[--input topic1 [topic2] ... [topicN]]\n");
  fprintf(stderr, "[--output topic1 [topic2] ... [topicN]]\n");
  fprintf(stderr, "[--help] print this help message\n");
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  CommandlineParser cmd(argc, argv, &usage);
  if (cmd.size() == 0) {
    fprintf(stderr, "iceoryx bridge needs a list of topics to bridge\n");
    return -1;
  }

  auto input_topics = cmd.get_option("--input");
  for (auto & topic : input_topics) {
    fprintf(stderr, "input topic %s\n", topic.c_str());
  }
  auto output_topics = cmd.get_option("--output");
  for (auto & topic : output_topics) {
    fprintf(stderr, "output topic %s\n", topic.c_str());
  }

  if (input_topics.empty() && output_topics.empty()) {
    fprintf(stderr, "Error: neither input nor output topics are specified\n");
    usage();
    return -1;
  }

  auto node_name = "iceoryx_ros2_bridge";
  auto node_options =
    rclcpp::NodeOptions().enable_rosout(false).start_parameter_services(false).
    start_parameter_event_publisher(
    false).enable_topic_statistics(false);
  auto node = std::make_shared<rclcpp::Node>(node_name, node_options);
  iox::runtime::PoshRuntime::getInstance(std::string("/") + node_name);

  /*
   * Subscribe to a ROS2 topic and re-publish into iceoryx
   */
  std::vector<std::shared_ptr<iceoryx_ros2_bridge::GenericSubscription>> ros2_subs;
  ros2_subs.reserve(input_topics.size());
  std::vector<std::shared_ptr<iox::popo::Publisher>> iceoryx_pubs;
  iceoryx_pubs.reserve(input_topics.size());

  rclcpp::sleep_for(2s);  // some room for ros2 discovery
  auto ros2_topic_names_and_types = node->get_topic_names_and_types();

  for (const auto & topic : input_topics) {
    auto pair = ros2_topic_names_and_types.find(topic);
    if (pair == ros2_topic_names_and_types.end()) {
      fprintf(stderr, "topic %s not found, skipping ...\n", topic.c_str());
      continue;
    }
    if (pair->second.size() == 0) {
      fprintf(stderr, "no type information found for topic %s, skipping ...\n", topic.c_str());
      continue;
    }

    auto type = pair->second[0];
    fprintf(stderr, "subscribing to ros2 topic %s with type %s\n", topic.c_str(), type.c_str());

    std::shared_ptr<rcpputils::SharedLibrary> library_generic_subscription;
    auto ts = iceoryx_ros2_bridge::get_typesupport(
      type, "rosidl_typesupport_cpp", library_generic_subscription);

    auto service_desc =
      rmw_iceoryx_cpp::get_iceoryx_service_description(topic, ts);
    iceoryx_pubs.emplace_back(
      std::make_shared<iox::popo::Publisher>(
        service_desc, iox::cxx::CString100(iox::cxx::TruncateToCapacity, node_name)));
    iceoryx_pubs.back()->offer();

    std::function<void(std::shared_ptr<rclcpp::SerializedMessage>)> cb =
      std::bind(&publish_to_iceoryx, std::placeholders::_1, ts, iceoryx_pubs.back());

    ros2_subs.emplace_back(
      std::make_shared<iceoryx_ros2_bridge::GenericSubscription>(
        node->get_node_base_interface().get(), *ts, topic, cb));
    node->get_node_topics_interface()->add_subscription(ros2_subs.back(), nullptr);
  }

  /*
   * Subscribe to an iceoryx topic and re-publish into ROS2
   */
  std::vector<std::shared_ptr<iox::popo::Subscriber>> iceoryx_subs;
  iceoryx_subs.reserve(output_topics.size());
  std::vector<std::shared_ptr<iceoryx_ros2_bridge::GenericPublisher>> ros2_pubs;

  auto iceoryx_topic_names_and_types = rmw_iceoryx_cpp::get_topic_names_and_types();

  for (const auto & topic : output_topics) {
    auto pair = iceoryx_topic_names_and_types.find(topic);
    if (pair == iceoryx_topic_names_and_types.end()) {
      fprintf(stderr, "topic %s not found, skipping ...\n", topic.c_str());
      continue;
    }
    if (pair->second.size() == 0) {
      fprintf(stderr, "no type information found for topic %s, skipping ...\n", topic.c_str());
      continue;
    }

    auto type = pair->second;
    fprintf(stderr, "subscribing to iceoryx topic %s with type %s\n", topic.c_str(), type.c_str());

    std::shared_ptr<rcpputils::SharedLibrary> library_generic_publisher;
    auto ts = iceoryx_ros2_bridge::get_typesupport(
      type, "rosidl_typesupport_cpp", library_generic_publisher);

    ros2_pubs.emplace_back(
      std::make_shared<iceoryx_ros2_bridge::GenericPublisher>(
        node->get_node_base_interface().get(), topic, *ts, library_generic_publisher));

    auto service_desc =
      rmw_iceoryx_cpp::get_iceoryx_service_description(topic, ts);
    iceoryx_subs.emplace_back(
      std::make_shared<iox::popo::Subscriber>(
        service_desc, iox::cxx::CString100(iox::cxx::TruncateToCapacity, node_name)));
    iceoryx_subs.back()->subscribe(10);  // TODO(karsten1987): find a decent queue size

    auto cb =
      std::bind(&publish_to_ros2, iceoryx_subs.back(), ts, ros2_pubs.back());

    iceoryx_subs.back()->setReceiveHandler(cb);
  }

  rclcpp::spin(node);

  rclcpp::shutdown();
  return 0;
}
