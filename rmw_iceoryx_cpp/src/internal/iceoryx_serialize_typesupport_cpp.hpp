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

#ifndef INTERNAL__ICEORYX_SERIALIZE_TYPESUPPORT_CPP_HPP_
#define INTERNAL__ICEORYX_SERIALIZE_TYPESUPPORT_CPP_HPP_

#include <iostream>

#include <array>
#include <cassert>
#include <string>
#include <vector>

#include "rosidl_typesupport_cpp/message_type_support.hpp"

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
void serialize_element(
  std::vector<char> & serialized_msg,
  const char * ros_message_field);

template<
  class T,
  uint32_t SizeT = sizeof(T)
>
void serialize_array(
  std::vector<char> & serialized_msg,
  const void * ros_message_field,
  uint32_t size);

template<
  class T,
  uint32_t SizeT = sizeof(T),
  class ContainerT = std::vector<T>
>
void serialize_sequence(
  std::vector<char> & serialized_msg,
  const void * ros_message_field);

// Implementation
template<
  class T,
  uint32_t SizeT
>
void serialize_element(
  std::vector<char> & serialized_msg,
  const char * ros_message_field)
{
  debug_log("serializing data element of %u bytes\n", SizeT);
  serialized_msg.insert(serialized_msg.end(), ros_message_field, ros_message_field + SizeT);
}

template<>
void serialize_element<std::string, sizeof(std::string)>(
  std::vector<char> & serialized_msg,
  const char * ros_message_field)
{
  serialize_sequence<char, sizeof(char), std::string>(serialized_msg, ros_message_field);
}

template<>
void serialize_element<std::wstring, sizeof(std::wstring)>(
  std::vector<char> & serialized_msg,
  const char * ros_message_field)
{
  serialize_sequence<wchar_t, sizeof(wchar_t), std::wstring>(serialized_msg, ros_message_field);
}

template<
  class T,
  uint32_t SizeT
>
void serialize_array(
  std::vector<char> & serialized_msg,
  const void * ros_message_field,
  uint32_t size)
{
  debug_log("serializing data array of size %u\n", size);
  auto array = reinterpret_cast<const std::array<T, 1> *>(ros_message_field);
  auto data_ptr = reinterpret_cast<const char *>(array->data());
  for (auto i = 0u; i < size; ++i) {
    serialize_element<T>(serialized_msg, data_ptr + i * SizeT);
  }
}

template<
  class T,
  uint32_t SizeT,
  class ContainerT
>
void serialize_sequence(std::vector<char> & serialized_msg, const void * ros_message_field)
{
  auto sequence = reinterpret_cast<const ContainerT *>(ros_message_field);
  uint32_t size = sequence->size();
  debug_log("serializing data sequence of size %u\n", size);

  push_sequence_size(serialized_msg, size);
  for (const T & t : *sequence) {
    const char * data = reinterpret_cast<const char *>(&t);
    serialize_element<T>(serialized_msg, data);
  }
}

template<typename T>
void serialize_message_field(
  const rosidl_typesupport_introspection_cpp::MessageMember * member,
  std::vector<char> & serialized_msg,
  const char * ros_message_field)
{
  debug_log("serializing message field %s\n", member->name_);
  if (!member->is_array_) {
    serialize_element<T>(serialized_msg, ros_message_field);
  } else if (member->array_size_ > 0 && !member->is_upper_bound_) {
    serialize_array<T>(serialized_msg, ros_message_field, member->array_size_);
  } else {
    serialize_sequence<T>(serialized_msg, ros_message_field);
  }
}

void serialize(
  const void * ros_message,
  const rosidl_typesupport_introspection_cpp::MessageMembers * members,
  std::vector<char> & serialized_msg)
{
  assert(members);
  assert(ros_message);

  for (uint32_t i = 0; i < members->member_count_; ++i) {
    const auto member = members->members_ + i;
    const char * ros_message_field = static_cast<const char *>(ros_message) + member->offset_;
    switch (member->type_id_) {
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOL:
        serialize_message_field<bool>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BYTE:
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
        serialize_message_field<uint8_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
        serialize_message_field<char>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT32:
        serialize_message_field<float>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT64:
        serialize_message_field<double>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
        serialize_message_field<int16_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
        serialize_message_field<uint16_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
        serialize_message_field<int32_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
        serialize_message_field<uint32_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
        serialize_message_field<int64_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
        serialize_message_field<uint64_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
        serialize_message_field<std::string>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING:
        serialize_message_field<std::wstring>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
        {
          // Iterate recursively over the complex ROS messages
          auto sub_members =
            static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(member->
            members_->data);

          const void * subros_message = nullptr;
          size_t sequence_size = 0;
          size_t sub_members_size = sub_members->size_of_;
          // It's a single message
          if (!member->is_array_) {
            subros_message = ros_message_field;
            sequence_size = 1;
            // It's a fixed size array of messages
          } else if (member->array_size_ > 0 && !member->is_upper_bound_) {
            subros_message = ros_message_field;
            sequence_size = member->array_size_;
            // It's a dynamic sequence of messages
          } else {
            // Cast current ros_message_field ptr as vector "definition"
            auto vector = reinterpret_cast<const std::vector<unsigned char> *>(ros_message_field);
            // Vector size points to content of vector and returns number of bytes
            // submembersize is the size of one element in the vector
            // (it is provided by type support)
            sequence_size = vector->size() / sub_members_size;
            if (member->is_upper_bound_ && sequence_size > member->array_size_) {
              throw std::runtime_error("vector overcomes the maximum length");
            }
            // create ptr to content of vector to enable recursion
            subros_message = reinterpret_cast<const void *>(vector->data());
            // store the number of elements
            push_sequence_size(serialized_msg, sequence_size);
          }

          debug_log("serializing message field %s\n", member->name_);
          for (auto index = 0u; index < sequence_size; ++index) {
            serialize(subros_message, sub_members, serialized_msg);
            subros_message = static_cast<const char *>(subros_message) + sub_members_size;
          }
        }
        break;
      default:
        throw std::runtime_error(std::string("unknown type:") + member->name_);
    }
  }
}

}  // namespace details_cpp
}  // namespace rmw_iceoryx_cpp
#endif  // INTERNAL__ICEORYX_SERIALIZE_TYPESUPPORT_CPP_HPP_
