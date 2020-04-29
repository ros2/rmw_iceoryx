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

#ifndef GENERIC_PUBLISHER_HPP_
#define GENERIC_PUBLISHER_HPP_

#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"

namespace rcpputils
{
class SharedLibrary;
}  // namespace rcpputils

namespace iceoryx_ros2_bridge
{

class GenericPublisher : public rclcpp::PublisherBase
{
public:
  GenericPublisher(
    rclcpp::node_interfaces::NodeBaseInterface * node_base,
    const std::string & topic,
    const rosidl_message_type_support_t & type_support,
    std::shared_ptr<rcpputils::SharedLibrary> library_handle);

  ~GenericPublisher() override = default;

  void publish(std::shared_ptr<rmw_serialized_message_t> message);

  void publish(const rmw_serialized_message_t * message);

private:
  std::shared_ptr<rcpputils::SharedLibrary> ts_library_handle_;
};

}  // namespace iceoryx_ros2_bridge

#endif  // GENERIC_PUBLISHER_HPP_
