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

#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdarg.h>
#include <string>
#include <tuple>
#include <vector>

#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"

#include "iceoryx_deserialize_typesupport_cpp.hpp"

namespace rmw_iceoryx_cpp
{
namespace details_cpp
{

static inline void debug_log(const char * format, ...)
{
  va_list args;
  va_start(args, format);
  fprintf(stderr, "[READING] ");
  vfprintf(stderr, format, args);
  va_end(args);
}

std::pair<const char *, uint32_t> load_array_size(const char * serialized_msg)
{
  // This is 64 bit aligned
  // REVIEW: Please discuss
  const uint32_t array_check = *reinterpret_cast<const uint32_t *>(serialized_msg);
  serialized_msg += sizeof(array_check);
  const uint32_t array_size = *reinterpret_cast<const uint32_t *>(serialized_msg);
  serialized_msg += sizeof(array_size);

  if (array_check != 101) {
    throw std::runtime_error("can't load array size: check failed");
  }
  debug_log("load array size of %u\n", array_size);
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
  if (member->array_size_ > 0 && !member->is_upper_bound_) {
    size_t array_elements = member->array_size_;
    subros_message = ros_message_field;
    return std::make_pair(serialized_msg, array_elements);
  }

  uint32_t vector_elements = 0;
  std::tie(serialized_msg, vector_elements) = load_array_size(serialized_msg);
  auto vector = reinterpret_cast<std::vector<unsigned char> *>(ros_message_field);

  vector->resize(vector_elements * sub_members_size);
  subros_message = reinterpret_cast<void *>(vector->data());
  return std::make_pair(serialized_msg, vector_elements);
}

template<
  class T,
  uint32_t SizeT = sizeof(T)
>
const char * deserialize_element(
  const char * serialized_msg,
  void * ros_message_field)
{
  T * data = reinterpret_cast<T *>(ros_message_field);
  memcpy(data, serialized_msg, SizeT);
  std::cout << "deserialized data: " << *data << std::endl;
  serialized_msg += SizeT;

  return serialized_msg;
}

template<>
const char * deserialize_element<std::string, sizeof(std::string)>(
  const char * serialized_msg,
  void * ros_message_field)
{
  uint32_t string_size = 0;
  std::tie(serialized_msg, string_size) = load_array_size(serialized_msg);

  if (string_size > 0) {
    auto string = reinterpret_cast<std::string *>(ros_message_field);
    string->clear();
    string->assign(serialized_msg, serialized_msg + string_size);

    debug_log("deserialized string '%s' with size %zu\n", string->c_str(), string->size());
    serialized_msg += string_size;
  }
  //std::vector<char> vec;
  //serialized_msg = copy_payload_array_cpp<char>(serialized_msg, &vec);

  //int size = 0;
  //for (char terminator : vec) {
  //  if (terminator == '\0') {
  //    break;
  //  }
  //  ++size;
  //}

  //if (vec.size() > 0) {
  //  data->insert(data->begin(), vec.begin(), vec.begin() + size);
  //}

  //T * data = reinterpret_cast<T *>(ros_message_field);
  //memcpy(data, serialized_msg, size);
  //serialized_msg += size;

  return serialized_msg;
}

template<>
const char * deserialize_element<std::wstring, sizeof(std::wstring)>(
  const char * serialized_msg,
  void * ros_message_field)
{
  uint32_t string_size = 0;
  std::tie(serialized_msg, string_size) = load_array_size(serialized_msg);

  if (string_size > 0) {
    auto string = reinterpret_cast<std::wstring *>(ros_message_field);
    string->reserve(string_size);
    for (auto i = 0u; i < string_size; ++i) {
      wchar_t c{};
      serialized_msg = deserialize_element<wchar_t>(serialized_msg, &c);
      string->push_back(c);
    }
    //string->assign(serialized_msg, serialized_msg + string_size);

    debug_log("deserialized wstring '%ls' with size %zu\n", string->c_str(), string->size());
    //serialized_msg += string_size * sizeof(wchar_t);
  }

  return serialized_msg;
}

template<
  class T,
  uint32_t SizeT = sizeof(T)
>
const char * deserialize_array(
  const char * serialized_msg,
  void * ros_message_field,
  uint32_t size)
{
  auto array = reinterpret_cast<std::array<T, 1> *>(ros_message_field);
  auto dataPtr = reinterpret_cast<char *>(array->data());
  for (auto i = 0u; i < size; ++i) {
    serialized_msg = deserialize_element<T>(serialized_msg, dataPtr + i * SizeT);
  }

  return serialized_msg;
}

template<
  class T,
  uint32_t SizeT = sizeof(T)
>
const char * deserialize_sequence(
  const char * serialized_msg, void * ros_message_field)
{
  uint32_t sequence_size = 0;
  std::tie(serialized_msg, sequence_size) = load_array_size(serialized_msg);
  if (sequence_size > 0) {
    // FIXME: add ", typename ContainerAllocator::template rebind<int8_t>::other"
    auto data = reinterpret_cast<std::vector<T> *>(ros_message_field);
    debug_log("resizing data sequence to %zu\n", sequence_size);
    data->resize(sequence_size);
    //auto dataPtr = data->data();
    for (auto i = 0u; i < sequence_size; ++i) {
      char * dataPtr = reinterpret_cast<char *>(&(data->at(i)));
      serialized_msg = deserialize_element<T>(serialized_msg, dataPtr);
      //serialized_msg = deserialize_element<T>(serialized_msg, &data[i]);
    }
  }
  //uint32_t char_size = SizeT * array_size;
  //memcpy(data->data(), serialized_msg, char_size);
  //serialized_msg += char_size;
  return serialized_msg;
}

template<>
const char * deserialize_sequence<bool, sizeof(bool)>(
    const char * serialized_msg, void * ros_message_field)
{
  uint32_t sequence_size = 0;
  std::tie(serialized_msg, sequence_size) = load_array_size(serialized_msg);

  auto data = reinterpret_cast<std::vector<bool> *>(ros_message_field);
  data->resize(sequence_size);

  for (auto i = 0u; i < sequence_size; ++i) {
    bool b{};
    serialized_msg = deserialize_element<bool>(serialized_msg, &b);
    data->at(i) = b;
  }

  //uint32_t size = 0;
  //uint32_t array_size = 0;

  //std::tie(serialized_msg, array_size) = load_array_size(serialized_msg);
  //std::tie(serialized_msg, size) = load_array_size(serialized_msg);

  //auto data = reinterpret_cast<std::vector<bool> *>(ros_message_field);
  //data->resize(array_size);

  //// Boolean arrays are treated specially for they are stored as single bits
  //memcpy(data->begin()._M_p, serialized_msg, size);
  //for (auto b : *data) {
  //  debug_log("extracted bool: %s\n", b ? "true" : "false");
  //}
  //serialized_msg += size;

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
    return serialized_msg = deserialize_array<T>(serialized_msg, ros_message_field, member->array_size_);
  } else {
    return serialized_msg = deserialize_sequence<T>(serialized_msg, ros_message_field);
  }
}

//template<>
//const char * copy_payload_cpp<std::string>(
//  const rosidl_typesupport_introspection_cpp::MessageMember * member, const char * serialized_msg,
//  void * ros_message_field)
//{
//  if (!member->is_array_) {
//    std::string * data = reinterpret_cast<std::string *>(ros_message_field);
//    std::vector<char> vec;
//    serialized_msg = copy_payload_array_cpp<char>(serialized_msg, &vec);
//
//    int size = 0;
//    for (char terminator : vec) {
//      if (terminator == '\0') {
//        break;
//      }
//      ++size;
//    }
//
//    if (vec.size() > 0) {
//      data->insert(data->begin(), vec.begin(), vec.begin() + size);
//    }
//  } else {
//    if (member->array_size_ > 0 && !member->is_upper_bound_) {
//      std::string * ros_message_field_data =
//        reinterpret_cast<std::array<std::string, 1> *>(ros_message_field)->data();
//      for (auto i = 0u; i < member->array_size_; ++i) {
//        std::vector<char> vec;
//        serialized_msg = copy_payload_array_cpp<char>(serialized_msg, &vec);
//        //vec.push_back('\0');
//        debug_log("extracted string %s with size %zu\n", vec.data(), vec.size());
//        //std::string & string_msg = reinterpret_cast<std::string &>(ros_message_field_data[i]);
//        //string_msg.insert(ros_message_field_data[i].begin(), vec.begin(), vec.end());
//        ros_message_field_data[i].insert(ros_message_field_data[i].begin(), vec.begin(), vec.end());
//      }
//    } else {
//      std::vector<std::string> * data =
//        reinterpret_cast<std::vector<std::string> *>(ros_message_field);
//      uint32_t array_size = 0;
//      std::tie(serialized_msg, array_size) = load_array_size(serialized_msg);
//      data->resize(array_size);
//      for (auto & data_element : *data) {
//        std::vector<char> vec;
//        serialized_msg = copy_payload_array_cpp<char>(serialized_msg, &vec);
//        data_element.insert(data_element.begin(), vec.begin(), vec.end());
//      }
//    }
//  }
//  return serialized_msg;
//}

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
        serialized_msg = deserialize_message_field<uint8_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
        serialized_msg = deserialize_message_field<char>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT32:
        serialized_msg = deserialize_message_field<float>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT64:
        serialized_msg = deserialize_message_field<double>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
        serialized_msg = deserialize_message_field<int16_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
        serialized_msg = deserialize_message_field<uint16_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
        serialized_msg = deserialize_message_field<int32_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
        serialized_msg = deserialize_message_field<uint32_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
        serialized_msg = deserialize_message_field<int64_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
        serialized_msg = deserialize_message_field<uint64_t>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
        serialized_msg = deserialize_message_field<std::string>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING:
        serialized_msg = deserialize_message_field<std::wstring>(member, serialized_msg, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
        {
          auto sub_members =
            (const rosidl_typesupport_introspection_cpp::MessageMembers *)member->members_->data;
          if (!member->is_array_) {
            serialized_msg = deserialize(serialized_msg, sub_members, ros_message_field);
          } else {
            fprintf(stderr, "[READING]type: %s, array_size: %zu, is_upper_bound %s\n", member->name_, member->array_size_, member->is_upper_bound_ ? "true" : "false");
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
        }
        //serialized_msg = copy_payload_cpp_ros_message(member, serialized_msg, ros_message_field);
        break;
      default:
        throw std::runtime_error(std::string("unknown type") + member->name_);
    }
  }
  return serialized_msg;
}

}  // namespace details_cpp
}  // namespace rmw_iceoryx_cpp
