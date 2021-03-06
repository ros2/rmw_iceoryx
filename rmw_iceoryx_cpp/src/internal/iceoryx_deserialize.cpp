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

#include "rosidl_typesupport_cpp/message_type_support.hpp"

#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"

#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"

#include "rmw_iceoryx_cpp/iceoryx_deserialize.hpp"

#include "./iceoryx_deserialize_typesupport_c.hpp"
#include "./iceoryx_deserialize_typesupport_cpp.hpp"

namespace rmw_iceoryx_cpp
{

void deserialize(
  const char * serialized_msg,
  const rosidl_message_type_support_t * type_supports,
  void * ros_message)
{
  // serialize with cpp typesupport
  auto ts_cpp = get_message_typesupport_handle(
    type_supports,
    rosidl_typesupport_introspection_cpp::typesupport_identifier);
  if (ts_cpp != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(ts_cpp->data);
    rmw_iceoryx_cpp::details_cpp::deserialize(serialized_msg, members, ros_message);
  }

  // serialize with c typesupport
  auto ts_c = get_message_typesupport_handle(
    type_supports,
    rosidl_typesupport_introspection_c__identifier);
  if (ts_c != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(ts_c->data);
    rmw_iceoryx_cpp::details_c::deserialize(serialized_msg, members, ros_message);
  }
}

}  // namespace rmw_iceoryx_cpp
