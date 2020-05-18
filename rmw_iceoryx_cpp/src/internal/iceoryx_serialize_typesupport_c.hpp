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

void store_array_size(std::vector<char> & payloadVector, uint32_t array_size);

size_t get_array_elememts_and_assign_ros_message_field_c(
  const rosidl_typesupport_introspection_c__MessageMember * member,
  const void * ros_message_field,
  void * & subros_message);

template<typename T>
void copy_data_array_c(std::vector<char> & payloadVector, const void * ros_message_field);

template<typename T>
void copy_data_fixed_array(
  std::vector<char> & payloadVector,
  const void * ros_message_field,
  uint32_t size);

template<typename T>
void copy_data_c(
  const rosidl_typesupport_introspection_c__MessageMember * member,
  std::vector<char> & payloadVector,
  const char * ros_message_field);

template<>
void copy_data_c<std::string>(
  const rosidl_typesupport_introspection_c__MessageMember * member,
  std::vector<char> & payloadVector,
  const char * ros_message_field);

void copy_data_c_ros_message(
  const rosidl_typesupport_introspection_c__MessageMember * member,
  std::vector<char> & payloadVector, const char * ros_message_field);

void serialize(
  const void * ros_message,
  const rosidl_typesupport_introspection_c__MessageMembers * members,
  std::vector<char> & payloadVector);

}  // namespace details_c
}  // namespace rmw_iceoryx_cpp
