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

#include <array>
#include <cassert>
#include <string>
#include <vector>

#include "rosidl_generator_c/primitives_sequence.h"

#include "rosidl_typesupport_cpp/message_type_support.hpp"

#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"

#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"

#include "rmw_iceoryx_cpp/iceoryx_serialize.hpp"

namespace rmw_iceoryx_cpp
{

void store_array_size(std::vector<char> & payloadVector, uint32_t array_size)
{
  const uint32_t check = 101;
  const char * sizePtr = reinterpret_cast<const char *>(&array_size);
  const char * checkPtr = reinterpret_cast<const char *>(&check);
  payloadVector.insert(payloadVector.end(), checkPtr, checkPtr + sizeof(check));
  payloadVector.insert(payloadVector.end(), sizePtr, sizePtr + sizeof(array_size));
}

size_t get_array_elememts_and_assign_ros_message_field_cpp(
  const rosidl_typesupport_introspection_cpp::MessageMember * member,
  const void * ros_message_field,
  void * & subros_message,
  size_t sub_members_size)
{
  // Cast current ros_message_field ptr as vector "definition"
  auto vector = reinterpret_cast<const std::vector<unsigned char> *>(ros_message_field);
  // Vector size points to content of vector and returns number of bytes
  // submembersize is th size of one element in the vector (it is provided by type support)
  size_t vector_elements = vector->size() / sub_members_size;
  if (member->is_upper_bound_ && vector_elements > member->array_size_) {
    throw std::runtime_error("vector overcomes the maximum length");
  }
  // create ptr to content of vector to enable recursion
  subros_message = const_cast<void *>(reinterpret_cast<const void *>(vector->data()));
  return vector_elements;
}

size_t get_array_elememts_and_assign_ros_message_field_c(
  const rosidl_typesupport_introspection_c__MessageMember * member,
  const void * ros_message_field,
  void * & subros_message
)
{
  auto data_array = reinterpret_cast<const rosidl_generator_c__char__Sequence *>(ros_message_field);
  uint32_t array_size = data_array->size;

  if (member->is_upper_bound_ && array_size > member->array_size_) {
    throw std::runtime_error("vector overcomes the maximum length");
  }
  // create ptr to content of vector to enable recursion
  subros_message = const_cast<void *>(reinterpret_cast<const void *>(data_array->data));
  return array_size;
}

template<typename T>
void copy_data_array_cpp(std::vector<char> & payloadVector, const void * ros_message_field)
{
  const std::vector<T> * data_array = reinterpret_cast<const std::vector<T> *>(ros_message_field);
  uint32_t array_size = data_array->size();
  uint32_t size = sizeof(T) * array_size;

  store_array_size(payloadVector, array_size);

  const char * arrayPtr = reinterpret_cast<const char *>(data_array->data());
  payloadVector.insert(payloadVector.end(), arrayPtr, arrayPtr + size);
}

template<>
void copy_data_array_cpp<bool>(std::vector<char> & payloadVector, const void * ros_message_field)
{
  const std::vector<bool> * data_array =
    reinterpret_cast<const std::vector<bool> *>(ros_message_field);
  // booleans are stored bit-wise this way it is possible to get the actual size of the array
  uint32_t size = data_array->end() - data_array->begin();

  store_array_size(payloadVector, data_array->size());  // number of booleans
  store_array_size(payloadVector, size);  // size in bytes

  payloadVector.insert(payloadVector.end(), data_array->begin(), data_array->end());
}

template<typename T>
void copy_data_array_c(std::vector<char> & payloadVector, const void * ros_message_field)
{
  auto data_array = reinterpret_cast<const rosidl_generator_c__char__Sequence *>(ros_message_field);
  uint32_t array_size = data_array->size;
  uint32_t size = sizeof(T) * array_size;

  store_array_size(payloadVector, array_size);

  const char * arrayPtr = reinterpret_cast<const char *>(data_array->data);
  payloadVector.insert(payloadVector.end(), arrayPtr, arrayPtr + size);
}

template<typename T>
void copy_data_fixed_array(
  std::vector<char> & payloadVector, const void * ros_message_field,
  uint32_t size)
{
  auto fixed_array = reinterpret_cast<const std::array<T, 1> *>(ros_message_field);
  auto dataPtr = reinterpret_cast<const char *>(fixed_array->data());
  payloadVector.insert(payloadVector.end(), dataPtr, dataPtr + size * sizeof(T));
}


template<typename T>
void copy_data_cpp(
  const rosidl_typesupport_introspection_cpp::MessageMember * member,
  std::vector<char> & payloadVector, const char * ros_message_field)
{
  if (!member->is_array_) {
    uint32_t size = sizeof(T);
    payloadVector.insert(payloadVector.end(), ros_message_field, ros_message_field + size);
  } else {
    if (member->array_size_ > 0 && !member->is_upper_bound_) {
      copy_data_fixed_array<T>(payloadVector, ros_message_field, member->array_size_);
    } else {
      copy_data_array_cpp<T>(payloadVector, ros_message_field);
    }
  }
}

template<typename T>
void copy_data_c(
  const rosidl_typesupport_introspection_c__MessageMember * member,
  std::vector<char> & payloadVector, const char * ros_message_field)
{
  if (!member->is_array_) {
    uint32_t size = sizeof(T);
    payloadVector.insert(payloadVector.end(), ros_message_field, ros_message_field + size);
  } else {
    if (member->array_size_ > 0 && !member->is_upper_bound_) {
      copy_data_fixed_array<T>(payloadVector, ros_message_field, member->array_size_);
    } else {
      copy_data_array_c<T>(payloadVector, ros_message_field);
    }
  }
}

template<>
void copy_data_c<std::string>(
  const rosidl_typesupport_introspection_c__MessageMember * member,
  std::vector<char> & payloadVector, const char * ros_message_field)
{
  if (!member->is_array_) {
    copy_data_array_c<char>(payloadVector, ros_message_field);
  } else {
    if (member->array_size_ > 0 && !member->is_upper_bound_) {
      auto data = reinterpret_cast<const std::array<std::string, 1> *>(ros_message_field);
      auto array_size = static_cast<uint32_t>(member->array_size_);
      for (auto i = 0u; i < array_size; ++i) {
        std::string data_element = data->data()[i];
        copy_data_array_c<char>(payloadVector, data_element.c_str());
      }
    } else {
      auto data_array =
        reinterpret_cast<const rosidl_generator_c__char__Sequence *>(ros_message_field);
      uint32_t array_size = data_array->size;  // number of strings in the array

      store_array_size(payloadVector, array_size);

      auto dataPtr = reinterpret_cast<const char *>(data_array->data);

      for (uint32_t i = 0; i < array_size; ++i) {
        copy_data_array_c<char>(payloadVector, dataPtr);
        dataPtr += sizeof(std::string);
      }
    }
  }
}

template<>
void copy_data_cpp<std::string>(
  const rosidl_typesupport_introspection_cpp::MessageMember * member,
  std::vector<char> & payloadVector, const char * ros_message_field)
{
  if (!member->is_array_) {
    const std::string * data = reinterpret_cast<const std::string *>(ros_message_field);
    std::vector<char> vec(data->begin(), data->end());
    // also add 0 termination
    // vec.push_back('\0');
    copy_data_array_cpp<char>(payloadVector, &vec);
  } else {
    if (member->array_size_ > 0 && !member->is_upper_bound_) {
      auto data = reinterpret_cast<const std::array<std::string, 1> *>(ros_message_field);
      auto array_size = static_cast<uint32_t>(member->array_size_);
      for (auto i = 0u; i < array_size; ++i) {
        std::string data_element = data->data()[i];
        std::vector<char> vec(data_element.begin(), data_element.end());
        // also add 0 termination
        // vec.push_back('\0');
        copy_data_array_cpp<char>(payloadVector, &vec);
      }
    } else {
      const std::vector<std::string> * data =
        reinterpret_cast<const std::vector<std::string> *>(ros_message_field);
      uint32_t array_size = data->size();
      store_array_size(payloadVector, array_size);
      for (auto data_element : *data) {
        std::vector<char> vec(data_element.begin(), data_element.end());
        // also add 0 termination
        // vec.push_back('\0');
        copy_data_array_cpp<char>(payloadVector, &vec);
      }
    }
  }
}

void copy_data_c_ros_message(
  const rosidl_typesupport_introspection_c__MessageMember * member,
  std::vector<char> & payloadVector, const char * ros_message_field)
{
  auto sub_members =
    static_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(member->members_->data);
  if (!member->is_array_) {
    serialize(ros_message_field, sub_members, payloadVector);
  } else {
    void * subros_message = nullptr;
    size_t sub_members_size = sub_members->size_of_;
    size_t array_elememts = get_array_elememts_and_assign_ros_message_field_c(
      member,
      ros_message_field,
      subros_message);

    store_array_size(payloadVector, array_elememts);

    for (size_t index = 0; index < array_elememts; ++index) {
      serialize(subros_message, sub_members, payloadVector);
      subros_message = static_cast<char *>(subros_message) + sub_members_size;
    }
  }
}

void copy_data_cpp_ros_message(
  const rosidl_typesupport_introspection_cpp::MessageMember * member,
  std::vector<char> & payloadVector, const char * ros_message_field)
{
  auto sub_members = static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(
    member->members_->data);
  if (!member->is_array_) {
    serialize(ros_message_field, sub_members, payloadVector);
  } else {
    void * subros_message = nullptr;
    size_t array_elements;
    size_t sub_members_size = sub_members->size_of_;

    array_elements = get_array_elememts_and_assign_ros_message_field_cpp(
      member, ros_message_field,
      subros_message,
      sub_members_size);
    store_array_size(payloadVector, array_elements);
    for (size_t index = 0; index < array_elements; ++index) {
      serialize(subros_message, sub_members, payloadVector);
      subros_message = static_cast<char *>(subros_message) + sub_members_size;
    }
  }
}

void serialize(
  const void * ros_message,
  const rosidl_typesupport_introspection_cpp::MessageMembers * members,
  std::vector<char> & payloadVector)
{
  assert(members);
  assert(ros_message);

  for (uint32_t i = 0; i < members->member_count_; ++i) {
    const auto member = members->members_ + i;
    const char * ros_message_field = static_cast<const char *>(ros_message) + member->offset_;
    switch (member->type_id_) {
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOL:
        copy_data_cpp<bool>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BYTE:
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
        copy_data_cpp<uint8_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
        copy_data_cpp<char>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT32:
        copy_data_cpp<float>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT64:
        copy_data_cpp<double>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
        copy_data_cpp<int16_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
        copy_data_cpp<uint16_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
        copy_data_cpp<int32_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
        copy_data_cpp<uint32_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
        copy_data_cpp<int64_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
        copy_data_cpp<uint64_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
        copy_data_cpp<std::string>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
        copy_data_cpp_ros_message(member, payloadVector, ros_message_field);
        break;
      default:
        throw std::runtime_error("unknown type");
    }
  }
}

void serialize(
  const void * ros_message,
  const rosidl_typesupport_introspection_c__MessageMembers * members,
  std::vector<char> & payloadVector)
{
  assert(members);
  assert(ros_message);

  for (uint32_t i = 0; i < members->member_count_; ++i) {
    const auto member = members->members_ + i;
    const char * ros_message_field = static_cast<const char *>(ros_message) + member->offset_;
    switch (member->type_id_) {
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_BOOL:
        copy_data_c<bool>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_BYTE:
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_UINT8:
        copy_data_c<uint8_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_CHAR:
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_INT8:
        copy_data_c<char>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT32:
        copy_data_c<float>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT64:
        copy_data_c<double>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_INT16:
        copy_data_c<int16_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_UINT16:
        copy_data_c<uint16_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_INT32:
        copy_data_c<int32_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_UINT32:
        copy_data_c<uint32_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_INT64:
        copy_data_c<int64_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_UINT64:
        copy_data_c<uint64_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_STRING:
        copy_data_c<std::string>(member, payloadVector, ros_message_field);

        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE:
        copy_data_c_ros_message(member, payloadVector, ros_message_field);
        break;
      default:
        throw std::runtime_error("unknown type");
    }
  }
}

}  // namespace rmw_iceoryx_cpp
