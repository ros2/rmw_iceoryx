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

#ifndef INTERNAL__ICEORYX_DESERIALIZE_TYPESUPPORT_C_HPP_
#define INTERNAL__ICEORYX_DESERIALIZE_TYPESUPPORT_C_HPP_

#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "rosidl_runtime_c/string_functions.h"

#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"

#include "./iceoryx_serialization_common.hpp"

namespace rmw_iceoryx_cpp
{
namespace details_c
{

template<
  class T,
  size_t SizeT = sizeof(T)
>
const char * deserialize_element(
  const char * serialized_msg,
  void * ros_message_field)
{
  T * data = reinterpret_cast<T *>(ros_message_field);
  memcpy(data, serialized_msg, SizeT);
  serialized_msg += SizeT;

  return serialized_msg;
}

template<>
const char * deserialize_element<rosidl_runtime_c__String, sizeof(rosidl_runtime_c__String)>(
  const char * serialized_msg,
  void * ros_message_field)
{
  uint32_t string_size = 0;
  std::tie(serialized_msg, string_size) = pop_sequence_size(serialized_msg);

  auto string = reinterpret_cast<rosidl_runtime_c__String *>(ros_message_field);
  // valgrind reports a memory leak here
  rosidl_runtime_c__String__assignn(string, serialized_msg, string_size);
  serialized_msg += string_size;
  return serialized_msg;
}

template<typename T>
const char * deserialize_array(
  const char * serialized_msg,
  void * ros_message_field,
  uint32_t size)
{
  auto array = reinterpret_cast<T *>(ros_message_field);
  for (size_t i = 0; i < size; ++i) {
    auto data = reinterpret_cast<char *>(&array[i]);
    serialized_msg = deserialize_element<T>(serialized_msg, data);
  }
  return serialized_msg;
}

template<
  class T,
  size_t SizeT = sizeof(T)
>
const char * deserialize_sequence(const char * serialized_msg, void * ros_message_field)
{
  uint32_t array_size = 0;
  std::tie(serialized_msg, array_size) = pop_sequence_size(serialized_msg);

  if (array_size > 0) {
    auto sequence = reinterpret_cast<typename traits::sequence_type<T>::type *>(ros_message_field);
    sequence->data = static_cast<T *>(calloc(array_size, SizeT));
    sequence->size = array_size;
    sequence->capacity = array_size;

    return deserialize_array<T>(serialized_msg, sequence->data, array_size);
  }
  return serialized_msg;
}

template<>
const char * deserialize_sequence<char, sizeof(char)>(
  const char * serialized_msg,
  void * ros_message_field)
{
  uint32_t array_size = 0;
  std::tie(serialized_msg, array_size) = pop_sequence_size(serialized_msg);

  if (array_size > 0) {
    auto sequence = reinterpret_cast<rosidl_runtime_c__char__Sequence *>(ros_message_field);
    sequence->data = static_cast<signed char *>(calloc(array_size, sizeof(char)));
    sequence->size = array_size;
    sequence->capacity = array_size;

    return deserialize_array<char>(serialized_msg, sequence->data, array_size);
  }
  return serialized_msg;
}

template<typename T>
const char * deserialize_message_field(
  const rosidl_typesupport_introspection_c__MessageMember * member,
  const char * serialized_msg, void * ros_message_field)
{
  debug_log("deserializing %s\n", member->name_);
  if (!member->is_array_) {
    return deserialize_element<T>(serialized_msg, ros_message_field);
  } else if (member->array_size_ > 0 && !member->is_upper_bound_) {
    return deserialize_array<T>(serialized_msg, ros_message_field, member->array_size_);
  } else {
    return deserialize_sequence<T>(serialized_msg, ros_message_field);
  }
}

const char * deserialize(
  const char * serialized_msg,
  const rosidl_typesupport_introspection_c__MessageMembers * members,
  void * ros_message)
{
  for (uint32_t i = 0; i < members->member_count_; ++i) {
    const auto * member = members->members_ + i;
    char * ros_message_field = static_cast<char *>(ros_message) + member->offset_;
    switch (member->type_id_) {
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_BOOL:
        serialized_msg = deserialize_message_field<bool>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_BYTE:
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_UINT8:
        serialized_msg = deserialize_message_field<uint8_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_CHAR:
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_INT8:
        serialized_msg = deserialize_message_field<char>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT32:
        serialized_msg =
          deserialize_message_field<float>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT64:
        serialized_msg =
          deserialize_message_field<double>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_INT16:
        serialized_msg = deserialize_message_field<int16_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_UINT16:
        serialized_msg = deserialize_message_field<uint16_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_INT32:
        serialized_msg = deserialize_message_field<int32_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_UINT32:
        serialized_msg = deserialize_message_field<uint32_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_INT64:
        serialized_msg = deserialize_message_field<int64_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_UINT64:
        serialized_msg = deserialize_message_field<uint64_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_STRING:
        serialized_msg = deserialize_message_field<rosidl_runtime_c__String>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE:
        {
          auto sub_members =
            (const rosidl_typesupport_introspection_c__MessageMembers *)member->members_->data;

          void * subros_message = nullptr;
          size_t sequence_size = 0;
          size_t sub_members_size = sub_members->size_of_;
          // It's a single message
          if (!member->is_array_) {
            subros_message = ros_message_field;
            sequence_size = 1;
          } else if (member->array_size_ > 0 && !member->is_upper_bound_) {
            subros_message = ros_message_field;
            sequence_size = member->array_size_;
          } else {
            debug_log("deserializing ROS message %s\n", member->name_);

            std::tie(serialized_msg, sequence_size) = pop_sequence_size(serialized_msg);

            auto sequence =
              const_cast<rosidl_runtime_c__char__Sequence *>(reinterpret_cast<const
              rosidl_runtime_c__char__Sequence
              *>(ros_message_field));

            subros_message = reinterpret_cast<void *>(sequence->data);
          }

          for (size_t index = 0; index < sequence_size; ++index) {
            serialized_msg = deserialize(serialized_msg, sub_members, subros_message);
            subros_message = static_cast<char *>(subros_message) + sub_members_size;
          }
        }
        break;
      default:
        throw std::runtime_error("unknown type");
    }
  }
  return serialized_msg;
}

}  // namespace details_c
}  // namespace rmw_iceoryx_cpp
#endif  // INTERNAL__ICEORYX_DESERIALIZE_TYPESUPPORT_C_HPP_
