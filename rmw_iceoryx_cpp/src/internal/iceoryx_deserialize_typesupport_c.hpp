// Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
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

#pragma once

#include <array>
#include <cassert>
#include <string>
#include <vector>

#include "rosidl_runtime_c/string_functions.h"

#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"

namespace rmw_iceoryx_cpp
{
namespace details_c
{

std::pair<const char *, uint32_t> load_array_size(const char * serialized_msg);

inline std::pair<const char *, size_t> get_submessage_array_c(
  const rosidl_typesupport_introspection_c__MessageMember * member,
  const char * serialized_msg,
  void * ros_message_field,
  void * & subros_message,
  size_t sub_members_size);

template<typename T>
const char * copy_payload_array_c(const char * serialized_msg, void * ros_message_field);

template<>
const char * copy_payload_array_c<std::string>(
  const char * serialized_msg,
  void * ros_message_field);

template<typename T>
const char * copy_payload_fixed_array_c(
  const char * serialized_msg, void * ros_message_field,
  uint32_t size);

template<typename T>
const char * copy_payload_c(
  const rosidl_typesupport_introspection_c__MessageMember * member,
  const char * serialized_msg, void * ros_message_field);

template<>
const char * copy_payload_c<std::string>(
  const rosidl_typesupport_introspection_c__MessageMember * member, const char * serialized_msg,
  void * ros_message_field);

const char * copy_payload_c_ros_message(
  const rosidl_typesupport_introspection_c__MessageMember * member, const char * serialized_msg,
  void * ros_message_field);

const char * deserialize(
  const char * serialized_msg,
  const rosidl_typesupport_introspection_c__MessageMembers * members,
  void * ros_message);

}  // namespace details_c
}  // namespace rmw_iceoryx_cpp
