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

#ifndef INTERNAL__ICEORYX_SERIALIZATION_COMMON_HPP_
#define INTERNAL__ICEORYX_SERIALIZATION_COMMON_HPP_

#include <stdarg.h>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace rmw_iceoryx_cpp
{

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

inline void push_sequence_size(std::vector<char> & payloadVector, uint32_t array_size)
{
  const uint32_t check = 101;
  const char * sizePtr = reinterpret_cast<const char *>(&array_size);
  const char * checkPtr = reinterpret_cast<const char *>(&check);
  payloadVector.insert(payloadVector.end(), checkPtr, checkPtr + sizeof(check));
  payloadVector.insert(payloadVector.end(), sizePtr, sizePtr + sizeof(array_size));
}

inline std::pair<const char *, uint32_t> pop_sequence_size(const char * serialized_msg)
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

namespace details_c
{
namespace traits
{

template<class T>
struct sequence_type;

template<>
struct sequence_type<bool>
{
  using type = rosidl_runtime_c__boolean__Sequence;
};

template<>
struct sequence_type<char>
{
  using type = rosidl_runtime_c__char__Sequence;
};

template<>
struct sequence_type<float>
{
  using type = rosidl_runtime_c__float__Sequence;
};

template<>
struct sequence_type<double>
{
  using type = rosidl_runtime_c__double__Sequence;
};

template<>
struct sequence_type<int8_t>
{
  using type = rosidl_runtime_c__int8__Sequence;
};

template<>
struct sequence_type<uint8_t>
{
  using type = rosidl_runtime_c__uint8__Sequence;
};

template<>
struct sequence_type<int16_t>
{
  using type = rosidl_runtime_c__int16__Sequence;
};

template<>
struct sequence_type<uint16_t>
{
  using type = rosidl_runtime_c__uint16__Sequence;
};

template<>
struct sequence_type<int32_t>
{
  using type = rosidl_runtime_c__int32__Sequence;
};

template<>
struct sequence_type<uint32_t>
{
  using type = rosidl_runtime_c__uint32__Sequence;
};

template<>
struct sequence_type<int64_t>
{
  using type = rosidl_runtime_c__int64__Sequence;
};

template<>
struct sequence_type<uint64_t>
{
  using type = rosidl_runtime_c__uint64__Sequence;
};

template<>
struct sequence_type<rosidl_runtime_c__String>
{
  using type = rosidl_runtime_c__String__Sequence;
};

}  // namespace traits
}  // namespace details_c
}  // namespace rmw_iceoryx_cpp
#endif  // INTERNAL__ICEORYX_SERIALIZATION_COMMON_HPP_
