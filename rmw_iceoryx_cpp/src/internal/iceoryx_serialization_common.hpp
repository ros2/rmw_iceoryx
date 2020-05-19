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

#include <stdarg.h>
#include <tuple>
#include <vector>

#define DEBUG_LOG 0

static inline void debug_log(const char * format, ...)
{
#if DEBUG_LOG
  va_list args;
  va_start(args, format);
  fprintf(stderr, "[READING] ");
  vfprintf(stderr, format, args);
  va_end(args);
#else
  (void) format;
#endif
}

inline void store_sequence_size(std::vector<char> & payloadVector, uint32_t array_size)
{
  const uint32_t check = 101;
  const char * sizePtr = reinterpret_cast<const char *>(&array_size);
  const char * checkPtr = reinterpret_cast<const char *>(&check);
  payloadVector.insert(payloadVector.end(), checkPtr, checkPtr + sizeof(check));
  payloadVector.insert(payloadVector.end(), sizePtr, sizePtr + sizeof(array_size));
}

inline std::pair<const char *, uint32_t> pop_array_size(const char * serialized_msg)
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
  std::tie(serialized_msg, vector_elements) = pop_array_size(serialized_msg);
  auto vector = reinterpret_cast<std::vector<unsigned char> *>(ros_message_field);

  vector->resize(vector_elements * sub_members_size);
  subros_message = reinterpret_cast<void *>(vector->data());
  return std::make_pair(serialized_msg, vector_elements);
}
