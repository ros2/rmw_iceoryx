// Copyright 2018, Bosch Software Innovations GmbH.
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

#include "generic_publisher.hpp"

#include <memory>
#include <string>

#include "rcpputils/shared_library.hpp"

namespace iceoryx_ros2_bridge
{

GenericPublisher::GenericPublisher(
  rclcpp::node_interfaces::NodeBaseInterface * node_base,
  const std::string & topic,
  const rosidl_message_type_support_t & type_support,
  std::shared_ptr<rcpputils::SharedLibrary> library_handle)
: rclcpp::PublisherBase(node_base, topic, type_support, rcl_publisher_get_default_options()),
  ts_library_handle_(library_handle)
{}

void GenericPublisher::publish(std::shared_ptr<rmw_serialized_message_t> message)
{
  return publish(message.get());
}

void GenericPublisher::publish(const rmw_serialized_message_t * message)
{
  auto return_code = rcl_publish_serialized_message(
    get_publisher_handle().get(), message, NULL);

  if (return_code != RCL_RET_OK) {
    rclcpp::exceptions::throw_from_rcl_error(return_code, "failed to publish serialized message");
  }
}

}  // namespace iceoryx_ros2_bridge
