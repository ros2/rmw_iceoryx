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

#include "rosidl_typesupport_cpp/message_type_support.hpp"

#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"

#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"

#include "rcutils/error_handling.h"

#include "rmw_iceoryx_cpp/iceoryx_deserialize.hpp"
#include "rmw_iceoryx_cpp/iceoryx_type_info_introspection.hpp"

#include "./iceoryx_deserialize_typesupport_c.hpp"
#include "./iceoryx_deserialize_typesupport_cpp.hpp"

namespace rmw_iceoryx_cpp
{

void deserialize(
  const char * serialized_msg,
  const rosidl_message_type_support_t * type_supports,
  void * ros_message)
{
  auto ts = get_type_support(type_supports);

  if (ts.first == TypeSupportLanguage::CPP) {
    auto members_cpp =
      static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(ts.second->data);
    rmw_iceoryx_cpp::details_cpp::deserialize(serialized_msg, members_cpp, ros_message);
  } else if (ts.first == TypeSupportLanguage::C) {
    auto members_c =
      static_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(ts.second->data);
    rmw_iceoryx_cpp::details_c::deserialize(serialized_msg, members_c, ros_message);
  }
}

}  // namespace rmw_iceoryx_cpp
