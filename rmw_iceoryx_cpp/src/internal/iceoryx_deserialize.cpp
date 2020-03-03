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
#include <cstring>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "rosidl_generator_c/primitives_sequence.h"

#include "rosidl_typesupport_cpp/message_type_support.hpp"

#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"

#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"

#include "rmw_iceoryx_cpp/iceoryx_deserialize.hpp"

namespace rmw_iceoryx_cpp
{

std::pair<const char *, uint32_t> load_array_size(const char * serialized_msg)
{
  // This is 64 bit aligned
  // REVIEW: Please discuss
  const uint32_t array_check = *reinterpret_cast<const uint32_t *>(serialized_msg);
  serialized_msg += sizeof(array_check);
  const uint32_t array_size = *reinterpret_cast<const uint32_t *>(serialized_msg);
  serialized_msg += sizeof(array_size);

  if (array_check != 101) {
    std::cerr << "deserialization failure: array size is " << array_size <<
      " and check failed! (" << array_check <<
      ")" << std::endl;
  }
  return std::make_pair(serialized_msg, array_size);
}

// FIXME: Use proper templating here! + add allocator handling
inline std::pair<const char *, size_t> get_submessage_vector_cpp(
  const rosidl_typesupport_introspection_cpp::MessageMember * member,
  const char * serialized_msg,
  char * ros_message_field,
  void * & subros_message,
  size_t sub_members_size)
{
  (void)member;
  uint32_t vector_elements = 0;
  std::tie(serialized_msg, vector_elements) = load_array_size(serialized_msg);
  auto vector = reinterpret_cast<std::vector<unsigned char> *>(ros_message_field);

  vector->resize(vector_elements * sub_members_size);
  subros_message = reinterpret_cast<void *>(vector->data());
  return std::make_pair(serialized_msg, vector_elements);
}

inline std::pair<const char *, size_t> get_submessage_array_c(
  const rosidl_typesupport_introspection_c__MessageMember * member,
  const char * serialized_msg,
  void * ros_message_field,
  void * & subros_message,
  size_t sub_members_size)
{
  (void)member;
  uint32_t array_elements = 0;
  std::tie(serialized_msg, array_elements) = load_array_size(serialized_msg);

  auto data_array =
    const_cast<rosidl_generator_c__char__Sequence *>(reinterpret_cast<const
    rosidl_generator_c__char__Sequence
    *>(ros_message_field));

  data_array->data = static_cast<signed char *>(calloc(array_elements, sub_members_size));
  data_array->capacity = array_elements;
  data_array->size = array_elements;

  subros_message = reinterpret_cast<void *>(data_array->data);

  return std::make_pair(serialized_msg, array_elements);
}


template<typename T>
const char * copy_payload_array_cpp(const char * serialized_msg, void * ros_message_field)
{
  uint32_t size = sizeof(T);
  uint32_t array_size = 0;
  std::tie(serialized_msg, array_size) = load_array_size(serialized_msg);
  if (array_size > 0) {
    // FIXME: add ", typename ContainerAllocator::template rebind<int8_t>::other"
    std::vector<T> * data = reinterpret_cast<std::vector<T> *>(ros_message_field);
    data->resize(array_size);
    uint32_t char_size = size * array_size;
    memcpy(data->data(), serialized_msg, char_size);
    serialized_msg += char_size;
  }
  return serialized_msg;
}

template<>
const char * copy_payload_array_cpp<bool>(const char * serialized_msg, void * ros_message_field)
{
  uint32_t size = 0;
  uint32_t array_size = 0;

  std::tie(serialized_msg, array_size) = load_array_size(serialized_msg);
  std::tie(serialized_msg, size) = load_array_size(serialized_msg);

  std::vector<bool> * data = reinterpret_cast<std::vector<bool> *>(ros_message_field);

  data->resize(array_size);

  // Boolean arrays are treated specially for they are stored as single bits
  memcpy(data->begin()._M_p, serialized_msg, size);
  serialized_msg += size;

  return serialized_msg;
}

template<typename T>
const char * copy_payload_array_c(const char * serialized_msg, void * ros_message_field)
{
  uint32_t size = sizeof(T);
  uint32_t array_size = 0;
  std::tie(serialized_msg, array_size) = load_array_size(serialized_msg);

  uint32_t char_size = size * array_size;

  auto data_array =
    const_cast<rosidl_generator_c__char__Sequence *>(reinterpret_cast<const
    rosidl_generator_c__char__Sequence
    *>(ros_message_field));

  data_array->data = static_cast<signed char *>(calloc(array_size, size));
  data_array->capacity = array_size;

  memcpy(data_array->data, serialized_msg, char_size);
  serialized_msg += char_size;

  data_array->capacity = array_size;
  data_array->size = array_size;
  return serialized_msg;
}

template<>
const char * copy_payload_array_c<std::string>(
  const char * serialized_msg,
  void * ros_message_field)
{
  serialized_msg = copy_payload_array_c<char>(serialized_msg, ros_message_field);

  auto data_array =
    const_cast<rosidl_generator_c__char__Sequence *>(reinterpret_cast<const
    rosidl_generator_c__char__Sequence
    *>(ros_message_field));

  --data_array->capacity;
  // Set size of string to \0
  auto array_size = strlen((const char *) const_cast<const signed char *>(data_array->data));
  data_array->size = array_size;

  return serialized_msg;
}

template<typename T>
const char * copy_payload_fixed_array_cpp(
  const char * serialized_msg, void * ros_message_field,
  uint32_t size)
{
  T * ros_message_field_data = reinterpret_cast<std::array<T, 1> *>(ros_message_field)->data();
  auto char_size = size * sizeof(T);
  memcpy(ros_message_field_data, serialized_msg, char_size);
  serialized_msg += char_size;

  return serialized_msg;
}

template<typename T>
const char * copy_payload_cpp(
  const rosidl_typesupport_introspection_cpp::MessageMember * member,
  const char * serialized_msg, void * ros_message_field)
{
  if (!member->is_array_) {
    uint32_t size = sizeof(T);
    T * data = reinterpret_cast<T *>(ros_message_field);
    memcpy(data, serialized_msg, size);
    serialized_msg += size;
  } else {
    if (member->array_size_ > 0 && !member->is_upper_bound_) {
      serialized_msg = copy_payload_fixed_array_cpp<T>(
        serialized_msg, ros_message_field,
        member->array_size_);
    } else {
      serialized_msg = copy_payload_array_cpp<T>(serialized_msg, ros_message_field);
    }
  }
  return serialized_msg;
}

template<>
const char * copy_payload_cpp<std::string>(
  const rosidl_typesupport_introspection_cpp::MessageMember * member, const char * serialized_msg,
  void * ros_message_field)
{
  if (!member->is_array_) {
    std::string * data = reinterpret_cast<std::string *>(ros_message_field);
    std::vector<char> vec;
    serialized_msg = copy_payload_array_cpp<char>(serialized_msg, &vec);

    int size = 0;
    for (char terminator : vec) {
      if (terminator == '\0') {
        break;
      }
      ++size;
    }

    if (vec.size() > 0) {
      data->insert(data->begin(), vec.begin(), vec.begin() + size);
    }
  } else {
    if (member->array_size_ > 0 && !member->is_upper_bound_) {
      std::string * ros_message_field_data =
        reinterpret_cast<std::array<std::string, 1> *>(ros_message_field)->data();
      for (auto i = 0u; i < member->array_size_; ++i) {
        std::vector<char> vec;
        serialized_msg = copy_payload_array_cpp<char>(serialized_msg, &vec);
        ros_message_field_data[i].insert(ros_message_field_data[i].begin(), vec.begin(), vec.end());
      }
    } else {
      std::vector<std::string> * data =
        reinterpret_cast<std::vector<std::string> *>(ros_message_field);
      uint32_t array_size = 0;
      std::tie(serialized_msg, array_size) = load_array_size(serialized_msg);
      data->resize(array_size);
      for (auto & data_element : *data) {
        std::vector<char> vec;
        serialized_msg = copy_payload_array_cpp<char>(serialized_msg, &vec);
        data_element.insert(data_element.begin(), vec.begin(), vec.end());
      }
    }
  }
  return serialized_msg;
}


template<typename T>
const char * copy_payload_c(
  const rosidl_typesupport_introspection_c__MessageMember * member,
  const char * serialized_msg, void * ros_message_field)
{
  if (!member->is_array_) {
    uint32_t size = sizeof(T);
    T * data = reinterpret_cast<T *>(ros_message_field);
    memcpy(data, serialized_msg, size);
    serialized_msg += size;
  } else {
    if (member->array_size_ > 0 && !member->is_upper_bound_) {
      serialized_msg = copy_payload_fixed_array_cpp<T>(
        serialized_msg, ros_message_field,
        member->array_size_);
    } else {
      serialized_msg = copy_payload_array_c<T>(serialized_msg, ros_message_field);
    }
  }
  return serialized_msg;
}

template<>
const char * copy_payload_c<std::string>(
  const rosidl_typesupport_introspection_c__MessageMember * member, const char * serialized_msg,
  void * ros_message_field)
{
  if (!member->is_array_) {
    serialized_msg = copy_payload_array_c<std::string>(serialized_msg, ros_message_field);
  } else {
    if (member->array_size_ > 0 && !member->is_upper_bound_) {
      std::string * ros_message_field_data =
        reinterpret_cast<std::array<std::string, 1> *>(ros_message_field)->data();
      for (auto i = 0u; i < member->array_size_; ++i) {
        serialized_msg = copy_payload_array_c<std::string>(serialized_msg, ros_message_field_data);
      }
    } else {
      uint32_t array_size = 0;
      std::tie(serialized_msg, array_size) = load_array_size(serialized_msg);
      for (uint32_t i = 0; i < array_size; ++i) {
        serialized_msg = copy_payload_array_c<std::string>(serialized_msg, ros_message_field);
      }
    }
  }
  return serialized_msg;
}

const char * copy_payload_c_ros_message(
  const rosidl_typesupport_introspection_c__MessageMember * member, const char * serialized_msg,
  void * ros_message_field)
{
  auto sub_members =
    (const rosidl_typesupport_introspection_c__MessageMembers *)member->members_->data;
  if (!member->is_array_) {
    serialized_msg = deserialize(serialized_msg, sub_members, ros_message_field);
  } else {
    void * subros_message = nullptr;
    size_t array_elememts = 0;
    size_t sub_members_size = sub_members->size_of_;

    if (member->array_size_ && !member->is_upper_bound_) {
      subros_message = ros_message_field;
      array_elememts = member->array_size_;
    } else {
      std::tie(serialized_msg, array_elememts) = get_submessage_array_c(
        member, serialized_msg, ros_message_field, subros_message, sub_members_size);
    }
    for (size_t index = 0; index < array_elememts; ++index) {
      serialized_msg = deserialize(serialized_msg, sub_members, subros_message);
      subros_message = static_cast<char *>(subros_message) + sub_members_size;
    }
  }
  return serialized_msg;
}

const char * copy_payload_cpp_ros_message(
  const rosidl_typesupport_introspection_cpp::MessageMember * member, const char * serialized_msg,
  char * ros_message_field)
{
  auto sub_members =
    (const rosidl_typesupport_introspection_cpp::MessageMembers *)member->members_->data;
  if (!member->is_array_) {
    serialized_msg = deserialize(serialized_msg, sub_members, ros_message_field);
  } else {
    void * subros_message = nullptr;
    size_t array_elememts = 0;
    size_t sub_members_size = sub_members->size_of_;

    std::tie(serialized_msg, array_elememts) = get_submessage_vector_cpp(
      member, serialized_msg, ros_message_field, subros_message, sub_members_size);

    for (size_t index = 0; index < array_elememts; ++index) {
      serialized_msg = deserialize(serialized_msg, sub_members, subros_message);
      subros_message = static_cast<char *>(subros_message) + sub_members_size;
    }
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
        serialized_msg = copy_payload_cpp<bool>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BYTE:
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
        serialized_msg = copy_payload_cpp<uint8_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
        serialized_msg = copy_payload_cpp<char>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT32:
        serialized_msg = copy_payload_cpp<float>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT64:
        serialized_msg = copy_payload_cpp<double>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
        serialized_msg = copy_payload_cpp<int16_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
        serialized_msg = copy_payload_cpp<uint16_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
        serialized_msg = copy_payload_cpp<int32_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
        serialized_msg = copy_payload_cpp<uint32_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
        serialized_msg = copy_payload_cpp<int64_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
        serialized_msg = copy_payload_cpp<uint64_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
        serialized_msg = copy_payload_cpp<std::string>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
        serialized_msg = copy_payload_cpp_ros_message(member, serialized_msg, ros_message_field);
        break;
      default:
        throw std::runtime_error("unknown type");
    }
  }
  return serialized_msg;
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
        serialized_msg = copy_payload_c<bool>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_BYTE:
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_UINT8:
        serialized_msg = copy_payload_c<uint8_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_CHAR:
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_INT8:
        serialized_msg = copy_payload_c<char>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT32:
        serialized_msg = copy_payload_c<float>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT64:
        serialized_msg = copy_payload_c<double>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_INT16:
        serialized_msg = copy_payload_c<int16_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_UINT16:
        serialized_msg = copy_payload_c<uint16_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_INT32:
        serialized_msg = copy_payload_c<int32_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_UINT32:
        serialized_msg = copy_payload_c<uint32_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_INT64:
        serialized_msg = copy_payload_c<int64_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_UINT64:
        serialized_msg = copy_payload_c<uint64_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_STRING:
        serialized_msg = copy_payload_c<std::string>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE:
        serialized_msg = copy_payload_c_ros_message(member, serialized_msg, ros_message_field);
        break;
      default:
        throw std::runtime_error("unknown type");
    }
  }
  return serialized_msg;
}

}  // namespace rmw_iceoryx_cpp
