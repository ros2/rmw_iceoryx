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

#ifndef INTERNAL__ICEORYX_DESERIALIZE_TYPESUPPORT_CPP_HPP_
#define INTERNAL__ICEORYX_DESERIALIZE_TYPESUPPORT_CPP_HPP_

#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"

#include "./iceoryx_serialization_common.hpp"

namespace rmw_iceoryx_cpp
{
namespace details_cpp
{

// Forward declarations
template<
  class T,
  uint32_t SizeT = sizeof(T)
>
const char * deserialize_element(
  const char * serialized_msg,
  void * ros_message_field);

template<
  class T,
  uint32_t SizeT = sizeof(T)
>
const char * deserialize_array(
  const char * serialized_msg,
  void * ros_message_field,
  uint32_t size);

template<
  class T,
  uint32_t SizeT = sizeof(T),
  class ContainerT = std::vector<T>
>
const char * deserialize_sequence(
  const char * serialized_msg,
  void * ros_message_field);

template<>
const char * deserialize_sequence<wchar_t, sizeof(wchar_t), std::wstring>(
  const char * serialized_msg, void * ros_message_field);

// Implementation
template<
  class T,
  uint32_t SizeT
>
const char * deserialize_element(
  const char * serialized_msg,
  void * ros_message_field)
{
  T * element = reinterpret_cast<T *>(ros_message_field);
  memcpy(element, serialized_msg, SizeT);
  debug_log("deserializing data element with %u bytes\n", SizeT);
  serialized_msg += SizeT;

  return serialized_msg;
}

template<>
const char * deserialize_element<std::string, sizeof(std::string)>(
  const char * serialized_msg,
  void * ros_message_field)
{
  return deserialize_sequence<char, sizeof(char), std::string>(serialized_msg, ros_message_field);
}

template<>
const char * deserialize_element<std::wstring, sizeof(std::wstring)>(
  const char * serialized_msg,
  void * ros_message_field)
{
  return deserialize_sequence<wchar_t, sizeof(wchar_t), std::wstring>(
    serialized_msg,
    ros_message_field);
}

template<
  class T,
  uint32_t SizeT
>
const char * deserialize_array(
  const char * serialized_msg,
  void * ros_message_field,
  uint32_t size)
{
  auto array = reinterpret_cast<std::array<T, 1> *>(ros_message_field);
  auto data_ptr = reinterpret_cast<char *>(array->data());
  debug_log("deserializing array of size %zu\n", size);
  for (auto i = 0u; i < size; ++i) {
    serialized_msg = deserialize_element<T>(serialized_msg, data_ptr + i * SizeT);
  }

  return serialized_msg;
}

template<
  class T,
  uint32_t SizeT,
  class ContainerT
>
const char * deserialize_sequence(
  const char * serialized_msg, void * ros_message_field)
{
  uint32_t sequence_size = 0;
  std::tie(serialized_msg, sequence_size) = pop_sequence_size(serialized_msg);
  if (sequence_size > 0) {
    debug_log("deserializigng data sequence of size %zu\n", sequence_size);
    auto sequence = reinterpret_cast<ContainerT *>(ros_message_field);
    sequence->resize(sequence_size);
    for (T & t : *sequence) {
      char * data = reinterpret_cast<char *>(&t);
      serialized_msg = deserialize_element<T>(serialized_msg, data);
    }
  }
  return serialized_msg;
}

// error: cannot bind non-const lvalue reference of type ‘bool&’ to an rvalue of type ‘bool’
template<>
const char * deserialize_sequence<bool, sizeof(bool), std::vector<bool>>(
  const char * serialized_msg, void * ros_message_field)
{
  uint32_t sequence_size = 0;
  std::tie(serialized_msg, sequence_size) = pop_sequence_size(serialized_msg);
  if (sequence_size > 0) {
    auto sequence = reinterpret_cast<std::vector<bool> *>(ros_message_field);
    debug_log("deserializing bool sequence of size %zu\n", sequence_size);
    sequence->resize(sequence_size);
    for (auto i = 0u; i < sequence_size; ++i) {
      bool b{};
      char * data = reinterpret_cast<char *>(&b);
      serialized_msg = deserialize_element<bool>(serialized_msg, data);
      sequence->at(i) = b;
    }
  }
  return serialized_msg;
}

// error: cannot bind non-const lvalue reference of type ‘bool&’ to an rvalue of type ‘bool’
template<>
const char * deserialize_sequence<wchar_t, sizeof(wchar_t), std::wstring>(
  const char * serialized_msg, void * ros_message_field)
{
  uint32_t sequence_size = 0;
  std::tie(serialized_msg, sequence_size) = pop_sequence_size(serialized_msg);
  if (sequence_size > 0) {
    debug_log("deserializing wstring sequence of size %zu\n", sequence_size);
    auto sequence = reinterpret_cast<std::wstring *>(ros_message_field);
    std::wstring str;
    str.resize(sequence_size);
    for (wchar_t & c : str) {
      char * data = reinterpret_cast<char *>(&c);
      serialized_msg = deserialize_element<wchar_t>(serialized_msg, data);
    }
    *sequence = str;
  }
  return serialized_msg;
}

template<typename T>
const char * deserialize_message_field(
  const rosidl_typesupport_introspection_cpp::MessageMember * member,
  const char * serialized_msg,
  void * ros_message_field)
{
  debug_log("deserializing message field %s\n", member->name_);
  if (!member->is_array_) {
    return deserialize_element<T>(serialized_msg, ros_message_field);
  } else if (member->array_size_ > 0 && !member->is_upper_bound_) {
    return serialized_msg = deserialize_array<T>(
             serialized_msg, ros_message_field,
             member->array_size_);
  } else {
    return serialized_msg = deserialize_sequence<T>(serialized_msg, ros_message_field);
  }
  return serialized_msg;
}

const char * deserialize(
  const char * serialized_msg,
  const rosidl_typesupport_introspection_cpp::MessageMembers * members,
  void * ros_message)
{
  for (uint32_t i = 0; i < members->member_count_; ++i) {
    const auto * member = members->members_ + i;
    char * ros_message_field = static_cast<char *>(ros_message) + member->offset_;
    switch (member->type_id_) {
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOL:
        serialized_msg = deserialize_message_field<bool>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BYTE:
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
        serialized_msg = deserialize_message_field<uint8_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
        serialized_msg = deserialize_message_field<char>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT32:
        serialized_msg =
          deserialize_message_field<float>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT64:
        serialized_msg =
          deserialize_message_field<double>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
        serialized_msg = deserialize_message_field<int16_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
        serialized_msg = deserialize_message_field<uint16_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
        serialized_msg = deserialize_message_field<int32_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
        serialized_msg = deserialize_message_field<uint32_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
        serialized_msg = deserialize_message_field<int64_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
        serialized_msg = deserialize_message_field<uint64_t>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
        serialized_msg = deserialize_message_field<std::string>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING:
        serialized_msg = deserialize_message_field<std::wstring>(
          member, serialized_msg,
          ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
        {
          auto sub_members =
            (const rosidl_typesupport_introspection_cpp::MessageMembers *)member->members_->data;

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

            auto sequence = reinterpret_cast<std::vector<unsigned char> *>(ros_message_field);
            sequence->resize(sequence_size * sub_members_size);
            subros_message = reinterpret_cast<void *>(sequence->data());
          }

          for (size_t index = 0; index < sequence_size; ++index) {
            serialized_msg = deserialize(serialized_msg, sub_members, subros_message);
            subros_message = static_cast<char *>(subros_message) + sub_members_size;
          }
        }
        break;
      default:
        throw std::runtime_error(std::string("unknown type") + member->name_);
    }
  }
  return serialized_msg;
}

}  // namespace details_cpp
}  // namespace rmw_iceoryx_cpp
#endif  // INTERNAL__ICEORYX_DESERIALIZE_TYPESUPPORT_CPP_HPP_
