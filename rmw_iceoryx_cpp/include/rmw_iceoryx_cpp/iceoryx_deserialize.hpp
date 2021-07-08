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

#ifndef RMW_ICEORYX_CPP__ICEORYX_DESERIALIZE_HPP_
#define RMW_ICEORYX_CPP__ICEORYX_DESERIALIZE_HPP_

struct rosidl_message_type_support_t;

namespace rmw_iceoryx_cpp
{

/// @todo karsten1987: This should be `uint8`, really
void deserialize(
  const char * serialized_msg,
  const rosidl_message_type_support_t * type_supports,
  void * ros_message);

}  // namespace rmw_iceoryx_cpp
#endif  // RMW_ICEORYX_CPP__ICEORYX_DESERIALIZE_HPP_
