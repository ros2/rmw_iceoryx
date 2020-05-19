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

#include <iostream>

#include <array>
#include <cassert>
#include <stdarg.h>
#include <string>
#include <vector>

#include "rosidl_typesupport_cpp/message_type_support.hpp"

#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"

#include "iceoryx_serialize_typesupport_cpp.hpp"

namespace rmw_iceoryx_cpp
{
namespace details_cpp
{

static inline void debug_log(const char * format, ...)
{
  setvbuf(stderr, NULL, _IONBF, BUFSIZ);
  va_list args;
  va_start(args, format);
  fprintf(stderr, "[WRITING] ");
  vfprintf(stderr, format, args);
  va_end(args);
}

void store_sequence_size(std::vector<char> & payloadVector, uint32_t array_size)
{
  debug_log("storing sequence indicator (%u)\n", array_size);
  const uint32_t check = 101;
  const char * sizePtr = reinterpret_cast<const char *>(&array_size);
  const char * checkPtr = reinterpret_cast<const char *>(&check);
  payloadVector.insert(payloadVector.end(), checkPtr, checkPtr + sizeof(check));
  payloadVector.insert(payloadVector.end(), sizePtr, sizePtr + sizeof(array_size));
}

template<
  class T,
  uint32_t SizeT = sizeof(T)
>
void serialize_element(
  std::vector<char> & payloadVector,
  const char * ros_message_field)
{
  debug_log("storing single data size %u\n", SizeT);
  std::cout << "stored data: " << *(reinterpret_cast<const T *>(ros_message_field)) << std::endl;
  payloadVector.insert(payloadVector.end(), ros_message_field, ros_message_field + SizeT);
}

template<>
void serialize_element<std::string, sizeof(std::string)>(
   std::vector<char> & payloadVector,
   const char * ros_message_field)
{
  auto string = reinterpret_cast<const std::string *>(ros_message_field);
  debug_log("storing string '%s' with size %zu\n", string->c_str(), string->size());
  store_sequence_size(payloadVector, string->size());
  payloadVector.insert(payloadVector.end(), string->begin(), string->end());
}

template<>
void serialize_element<std::wstring, sizeof(std::wstring)>(
   std::vector<char> & payloadVector,
   const char * ros_message_field)
{
  auto string = reinterpret_cast<const std::wstring *>(ros_message_field);
  debug_log("storing wstring '%ls' with size %zu\n", string->c_str(), string->size());
  store_sequence_size(payloadVector, string->size());
  for (const wchar_t & c : *string) {
    auto data = reinterpret_cast<const char *>(&c);
    serialize_element<wchar_t>(payloadVector, data);
  }
}

template<
  class T,
  uint32_t SizeT = sizeof(T)
>
void serialize_array(
  std::vector<char> & payloadVector,
  const void * ros_message_field,
  uint32_t size)
{
  auto array = reinterpret_cast<const std::array<T, 1> *>(ros_message_field);
  auto dataPtr = reinterpret_cast<const char *>(array->data());
  debug_log("storing data array of size %u\n", size * SizeT);
  for (auto i = 0u; i < size; ++i) {
    serialize_element<T>(payloadVector, dataPtr + i * SizeT);
  }
}

template<
  class T,
  uint32_t SizeT = sizeof(T)
>
void serialize_sequence(std::vector<char> & payloadVector, const void * ros_message_field)
{
  auto sequence = reinterpret_cast<const std::vector<T> *>(ros_message_field);
  uint32_t size = sequence->size();

  store_sequence_size(payloadVector, size);

  debug_log("storing data sequence of size %u\n", size);
  for (auto i = 0u; i < size; ++i) {
    const char * dataPtr = reinterpret_cast<const char *>(&(sequence->at(i)));
    serialize_element<T>(payloadVector, dataPtr);
  }
}

template<>
void serialize_sequence<bool, sizeof(bool)>(std::vector<char> & payloadVector, const void * ros_message_field)
{
  const std::vector<bool> * sequence =
    reinterpret_cast<const std::vector<bool> *>(ros_message_field);
  uint32_t size = sequence->size();
  store_sequence_size(payloadVector, size);

  for (auto i = 0u; i < size; ++i) {
    bool b = sequence->at(i);
    auto data = reinterpret_cast<const char *>(&b);
    serialize_element<bool>(payloadVector, data);
  }
  //const std::vector<bool> * data_array =
  //  reinterpret_cast<const std::vector<bool> *>(ros_message_field);
  //// booleans are stored bit-wise this way it is possible to get the actual size of the array
  //uint32_t size = data_array->end() - data_array->begin();

  //store_sequence_size(payloadVector, data_array->size());  // number of booleans
  //store_sequence_size(payloadVector, size);  // size in bytes

  //debug_log("storing bool sequence of size %u\n", size);
  //payloadVector.insert(payloadVector.end(), data_array->begin(), data_array->end());
  //(void) payloadVector;
  //(void) ros_message_field;
}

template<typename T>
void serialize_message_field(
  const rosidl_typesupport_introspection_cpp::MessageMember * member,
  std::vector<char> & payloadVector,
  const char * ros_message_field)
{
  debug_log("serializing message field %s\n", member->name_);
  if (!member->is_array_) {
    serialize_element<T>(payloadVector, ros_message_field);
  } else if (member->array_size_ > 0 && !member->is_upper_bound_) {
    serialize_array<T>(payloadVector, ros_message_field, member->array_size_);
  } else {
    serialize_sequence<T>(payloadVector, ros_message_field);
  }
}

//template<>
//void serialize_message_field<std::string>(
//  const rosidl_typesupport_introspection_cpp::MessageMember * member,
//  std::vector<char> & payloadVector,
//  const char * ros_message_field)
//{
//  if (!member->is_array_) {
//    serialize_element<std::string>(payloadVector, ros_message_field);
//  } else if (member->array_size_ > 0 && !member->is_upper_bound_) {
//      auto data = reinterpret_cast<const std::array<std::string, 1> *>(ros_message_field);
//      auto array_size = static_cast<uint32_t>(member->array_size_);
//      for (auto i = 0u; i < array_size; ++i) {
//        std::string data_element = data->data()[i];
//        std::vector<char> vec(data_element.begin(), data_element.end());
//        // also add 0 termination
//        // vec.push_back('\0');
//        serialize_sequence<char>(payloadVector, &vec);
//      }
//    } else {
//      const std::vector<std::string> * data =
//        reinterpret_cast<const std::vector<std::string> *>(ros_message_field);
//      uint32_t array_size = data->size();
//      store_sequence_size(payloadVector, array_size);
//      for (auto data_element : *data) {
//        std::vector<char> vec(data_element.begin(), data_element.end());
//        // also add 0 termination
//        // vec.push_back('\0');
//        serialize_sequence<char>(payloadVector, &vec);
//      }
//    }
//  }
//}

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
        serialize_message_field<bool>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BYTE:
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
        serialize_message_field<uint8_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
        serialize_message_field<char>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT32:
        serialize_message_field<float>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT64:
        serialize_message_field<double>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
        serialize_message_field<int16_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
        serialize_message_field<uint16_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
        serialize_message_field<int32_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
        serialize_message_field<uint32_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
        serialize_message_field<int64_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
        serialize_message_field<uint64_t>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
        serialize_message_field<std::string>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING:
        serialize_message_field<std::wstring>(member, payloadVector, ros_message_field);
        break;
      case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
        {
        // Iterate recursively over the complex ROS messages
        auto sub_members =
          static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(member->members_->data);

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
          // submembersize is the size of one element in the vector (it is provided by type support)
          sequence_size = vector->size() / sub_members_size;
          if (member->is_upper_bound_ && sequence_size > member->array_size_) {
            throw std::runtime_error("vector overcomes the maximum length");
          }
          // create ptr to content of vector to enable recursion
          subros_message = reinterpret_cast<const void *>(vector->data());
          // store the number of elements
          store_sequence_size(payloadVector, sequence_size);
        }

        debug_log("\tserializing message field %s\n", member->name_);
        //debug_log("type: %s, array_size: %zu, is_upper_bound %s\n",
        //  member->name_, member->array_size_, member->is_upper_bound_ ? "true" : "false");
        //debug_log("adding %zu elements to vector\n", sequence_size);
        for (auto index = 0u; index < sequence_size; ++index) {
          serialize(subros_message, sub_members, payloadVector);
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
