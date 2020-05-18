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
#include <string>
#include <vector>

namespace rosidl_typesupport_introspection_cpp
{
struct MessageMember;
}  // namespace rosidl_typesupport_introspection_cpp

namespace rmw_iceoryx_cpp
{
namespace details_cpp
{

void store_sequence_size(std::vector<char> & payloadVector, uint32_t array_size);

size_t get_array_elememts_and_assign_ros_message_field_cpp(
  const rosidl_typesupport_introspection_cpp::MessageMember * member,
  const void * ros_message_field,
  void * & subros_message,
  size_t sub_members_size);

/// Serialize dynamic sequences of messages
/**
 * A sequence can be either bounded or unbounded.
 * In both cases, the sequence is represented as a
 * std::vector<T> within the C++ typesupport context.
 */
template<
  class T,
  uint32_t SizeT = sizeof(T)
>
void serialize_sequence(std::vector<char> & payloadVector, const void * ros_message_field);

//template<>
//void serialize_sequence<bool>(std::vector<char> & payloadVector, const void * ros_message_field);

/// Serialize arrays of messages
/**
 * An array is a fixed-size sequence of messages.
 * The size is defined in the typesupport and no size
 * indicator has to be stored before the array payload.
 * The array is being represented as a std::array<T,n>
 * within the C++ typesupport context.
 */
template<
  class T,
  uint32_t SizeT = sizeof(T)
>
void serialize_array(
  std::vector<char> & payloadVector,
  const void * ros_message_field,
  uint32_t size);

/// Serialize a single data element
/**
 * A data element can only a plain integral value.
 */
template<
  class T,
  uint32_t SizeT = sizeof(T)
>
void serialize_element(
  std::vector<char> & payloadVector,
  const char * ros_message_field);

void serialize(
  const void * ros_message,
  const rosidl_typesupport_introspection_cpp::MessageMembers * members,
  std::vector<char> & payloadVector);

}  // namespace details_cpp
}  // namespace rmw_iceoryx_cpp
