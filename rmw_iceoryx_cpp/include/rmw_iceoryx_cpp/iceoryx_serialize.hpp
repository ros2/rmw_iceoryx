// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2023 by Apex.AI Inc. All rights reserved.
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

#ifndef RMW_ICEORYX_CPP__ICEORYX_SERIALIZE_HPP_
#define RMW_ICEORYX_CPP__ICEORYX_SERIALIZE_HPP_

#include <vector>

struct rosidl_message_type_support_t;
struct rosidl_service_type_support_t;

namespace rmw_iceoryx_cpp
{

void serialize(
  const void * ros_message,
  const rosidl_message_type_support_t * type_supports,
  std::vector<char> & payload_vector);

void serializeRequest(
  const void * ros_message,
  const rosidl_service_type_support_t * type_supports,
  std::vector<char> & payload_vector);

void serializeResponse(
  const void * ros_message,
  const rosidl_service_type_support_t * type_supports,
  std::vector<char> & payload_vector);

}  // namespace rmw_iceoryx_cpp
#endif  // RMW_ICEORYX_CPP__ICEORYX_SERIALIZE_HPP_
