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

#ifndef TEST_MSGS_C_FIXTURES_HPP_
#define TEST_MSGS_C_FIXTURES_HPP_

#include <memory>
#include <limits>
#include <string>
#include <vector>

#include "rosidl_runtime_c/string_functions.h"
#include "rosidl_runtime_c/primitives_sequence_functions.h"

#include "test_msgs/msg/arrays.h"
#include "test_msgs/msg/basic_types.h"
#include "test_msgs/msg/bounded_sequences.h"
#include "test_msgs/msg/builtins.h"
#include "test_msgs/msg/constants.h"
#include "test_msgs/msg/defaults.h"
#include "test_msgs/msg/empty.h"
#include "test_msgs/msg/multi_nested.h"
#include "test_msgs/msg/nested.h"
#include "test_msgs/msg/strings.h"
#include "test_msgs/msg/unbounded_sequences.h"

std::vector<std::shared_ptr<test_msgs__msg__Empty>>
get_messages_empty_c()
{
  using T = test_msgs__msg__Empty;
  std::vector<std::shared_ptr<T>> vec{};
  {
    auto empty = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__Empty__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__Empty__init(empty.get());
    vec.push_back(empty);
  }
  return vec;
}

bool operator==(const test_msgs__msg__Empty & lhs, const test_msgs__msg__Empty & rhs)
{
  (void) lhs;
  (void) rhs;
  return true;
}

bool operator!=(const test_msgs__msg__Empty & lhs, const test_msgs__msg__Empty & rhs)
{
  return !(lhs == rhs);
}

std::vector<std::shared_ptr<test_msgs__msg__BasicTypes>>
get_messages_basic_types_c()
{
  using T = test_msgs__msg__BasicTypes;
  std::vector<std::shared_ptr<T>> vec{};
  {
    auto basic_type = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__BasicTypes__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__BasicTypes__init(basic_type.get());
    basic_type->bool_value = false;
    basic_type->byte_value = 0;
    basic_type->char_value = '\0';
    basic_type->float32_value = 0.0f;
    basic_type->float64_value = 0;
    basic_type->int8_value = 0;
    basic_type->uint8_value = 0;
    basic_type->int16_value = 0;
    basic_type->uint16_value = 0;
    basic_type->int32_value = 0;
    basic_type->uint32_value = 0;
    basic_type->int64_value = 0;
    basic_type->uint64_value = 0;

    vec.push_back(basic_type);
  }
  {
    auto basic_type = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__BasicTypes__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__BasicTypes__init(basic_type.get());
    basic_type->bool_value = true;
    basic_type->byte_value = 255;
    basic_type->char_value = '\x7f';
    basic_type->float32_value = 1.125f;
    basic_type->float64_value = 1.125;
    basic_type->int8_value = (std::numeric_limits<int8_t>::max)();
    basic_type->uint8_value = (std::numeric_limits<uint8_t>::max)();
    basic_type->int16_value = (std::numeric_limits<int16_t>::max)();
    basic_type->uint16_value = (std::numeric_limits<uint16_t>::max)();
    basic_type->int32_value = (std::numeric_limits<int32_t>::max)();
    basic_type->uint32_value = (std::numeric_limits<uint32_t>::max)();
    basic_type->int64_value = (std::numeric_limits<int64_t>::max)();
    basic_type->uint64_value = (std::numeric_limits<uint64_t>::max)();

    vec.push_back(basic_type);
  }
  {
    auto basic_type = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__BasicTypes__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__BasicTypes__init(basic_type.get());
    basic_type->bool_value = false;
    basic_type->byte_value = 0;
    basic_type->char_value = 0x0;
    basic_type->float32_value = -2.125f;
    basic_type->float64_value = -2.125;
    basic_type->int8_value = (std::numeric_limits<int8_t>::min)();
    basic_type->uint8_value = 0;
    basic_type->int16_value = (std::numeric_limits<int16_t>::min)();
    basic_type->uint16_value = 0;
    basic_type->int32_value = (std::numeric_limits<int32_t>::min)();
    basic_type->uint32_value = 0;
    basic_type->int64_value = (std::numeric_limits<int64_t>::min)();
    basic_type->uint64_value = 0;

    vec.push_back(basic_type);
  }
  {
    auto basic_type = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__BasicTypes__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__BasicTypes__init(basic_type.get());
    basic_type->bool_value = true;
    basic_type->byte_value = 1;
    basic_type->char_value = '\1';
    basic_type->float32_value = 1.0f;
    basic_type->float64_value = 1;
    basic_type->int8_value = 1;
    basic_type->uint8_value = 1;
    basic_type->int16_value = 1;
    basic_type->uint16_value = 1;
    basic_type->int32_value = 1;
    basic_type->uint32_value = 1;
    basic_type->int64_value = 1;
    basic_type->uint64_value = 1;

    vec.push_back(basic_type);
  }

  return vec;
}

bool operator==(const test_msgs__msg__BasicTypes & lhs, const test_msgs__msg__BasicTypes & rhs)
{
  if (lhs.bool_value != rhs.bool_value) {return false;}
  if (lhs.byte_value != rhs.byte_value) {return false;}
  if (lhs.char_value != rhs.char_value) {return false;}
  if (lhs.float32_value != rhs.float32_value) {return false;}
  if (lhs.float64_value != rhs.float64_value) {return false;}
  if (lhs.int8_value != rhs.int8_value) {return false;}
  if (lhs.uint8_value != rhs.uint8_value) {return false;}
  if (lhs.int16_value != rhs.int16_value) {return false;}
  if (lhs.uint16_value != rhs.uint16_value) {return false;}
  if (lhs.int32_value != rhs.int32_value) {return false;}
  if (lhs.uint32_value != rhs.uint32_value) {return false;}
  if (lhs.int64_value != rhs.int64_value) {return false;}
  if (lhs.uint64_value != rhs.uint64_value) {return false;}

  return true;
}

bool operator!=(const test_msgs__msg__BasicTypes & lhs, const test_msgs__msg__BasicTypes & rhs)
{
  return !(lhs == rhs);
}

std::vector<std::shared_ptr<test_msgs__msg__Constants>>
get_messages_constants_c()
{
  using T = test_msgs__msg__Constants;
  std::vector<std::shared_ptr<T>> vec{};
  {
    auto constants = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__Constants__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__Constants__init(constants.get());
    vec.push_back(constants);
  }

  return vec;
}

bool operator==(const test_msgs__msg__Constants & lhs, const test_msgs__msg__Constants & rhs)
{
  (void) lhs;
  (void) rhs;

  return true;
}

bool operator!=(const test_msgs__msg__Constants & lhs, const test_msgs__msg__Constants & rhs)
{
  return !(lhs == rhs);
}

std::vector<std::shared_ptr<test_msgs__msg__Defaults>>
get_messages_defaults_c()
{
  using T = test_msgs__msg__Defaults;
  std::vector<std::shared_ptr<T>> vec{};
  {
    auto defaults = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__Defaults__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__Defaults__init(defaults.get());
    vec.push_back(defaults);
  }

  return vec;
}

bool operator==(const test_msgs__msg__Defaults & lhs, const test_msgs__msg__Defaults & rhs)
{
  if (lhs.bool_value != rhs.bool_value) {return false;}
  if (lhs.byte_value != rhs.byte_value) {return false;}
  if (lhs.char_value != rhs.char_value) {return false;}
  if (lhs.float32_value != rhs.float32_value) {return false;}
  if (lhs.float64_value != rhs.float64_value) {return false;}
  if (lhs.int8_value != rhs.int8_value) {return false;}
  if (lhs.uint8_value != rhs.uint8_value) {return false;}
  if (lhs.int16_value != rhs.int16_value) {return false;}
  if (lhs.uint16_value != rhs.uint16_value) {return false;}
  if (lhs.int32_value != rhs.int32_value) {return false;}
  if (lhs.uint32_value != rhs.uint32_value) {return false;}
  if (lhs.int64_value != rhs.int64_value) {return false;}
  if (lhs.uint64_value != rhs.uint64_value) {return false;}

  return true;
}

bool operator!=(const test_msgs__msg__Defaults & lhs, const test_msgs__msg__Defaults & rhs)
{
  return !(lhs == rhs);
}

std::vector<std::shared_ptr<test_msgs__msg__Strings>>
get_messages_strings_c()
{
  using T = test_msgs__msg__Strings;
  std::vector<std::shared_ptr<T>> vec{};
  {
    auto string = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__Strings__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__Strings__init(string.get());
    rosidl_runtime_c__String__assign(&string->string_value, "");
    rosidl_runtime_c__String__assign(&string->bounded_string_value, "");
    vec.push_back(string);
  }
  {
    auto string = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__Strings__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__Strings__init(string.get());
    rosidl_runtime_c__String__assign(&string->string_value, "Hello world!");
    rosidl_runtime_c__String__assign(&string->bounded_string_value, "Hello world!");
    vec.push_back(string);
  }
  {
    auto string = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__Strings__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__Strings__init(string.get());
    rosidl_runtime_c__String__assign(&string->string_value, u8"Hell\u00F6 W\u00F6rld!");
    rosidl_runtime_c__String__assign(&string->bounded_string_value, u8"Hell\u00F6 W\u00F6rld!");
    vec.push_back(string);
  }
  {
    auto string = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__Strings__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__Strings__init(string.get());
    char string_value[20000] = {};
    for (uint32_t i = 0; i < 20000; i++) {
      string_value[i] = '0' + (i % 10);
    }
    char bounded_string_value[22] = {};
    for (uint32_t i = 0; i < 22; i++) {
      bounded_string_value[i] = '0' + (i % 10);
    }
    rosidl_runtime_c__String__assignn(&string->string_value, string_value, sizeof(string_value));
    rosidl_runtime_c__String__assignn(
      &string->bounded_string_value, bounded_string_value, sizeof(bounded_string_value));
    vec.push_back(string);
  }

  return vec;
}

bool operator==(const test_msgs__msg__Strings & lhs, const test_msgs__msg__Strings & rhs)
{
  if (0 != strcmp(lhs.string_value.data, rhs.string_value.data)) {return false;}
  if (0 != strcmp(lhs.bounded_string_value.data, rhs.bounded_string_value.data)) {return false;}

  return true;
}

bool operator!=(const test_msgs__msg__Strings & lhs, const test_msgs__msg__Strings & rhs)
{
  return !(lhs == rhs);
}

std::vector<std::shared_ptr<test_msgs__msg__Arrays>>
get_messages_arrays_c()
{
  using T = test_msgs__msg__Arrays;
  std::vector<std::shared_ptr<T>> vec{};
  {
    auto arrays = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__Arrays__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__Arrays__init(arrays.get());
    arrays->bool_values[0] = false;
    arrays->bool_values[1] = true;
    arrays->bool_values[2] = false;
    arrays->byte_values[0] = 0;
    arrays->byte_values[1] = 0xff;
    arrays->byte_values[2] = 0;
    arrays->char_values[0] = 0;
    arrays->char_values[1] = 255;
    arrays->char_values[2] = 0;
    arrays->float32_values[0] = 0.0f;
    arrays->float32_values[1] = 1.125f;
    arrays->float32_values[2] = -2.125f;
    arrays->float64_values[0] = 0;
    arrays->float64_values[1] = 1.125;
    arrays->float64_values[2] = -2.125;
    arrays->int8_values[0] = 0;
    arrays->int8_values[1] = (std::numeric_limits<int8_t>::max)();
    arrays->int8_values[2] = (std::numeric_limits<int8_t>::min)();
    arrays->uint8_values[0] = 0;
    arrays->uint8_values[1] = (std::numeric_limits<uint8_t>::max)();
    arrays->uint8_values[2] = 0;
    arrays->int16_values[0] = 0;
    arrays->int16_values[1] = (std::numeric_limits<int16_t>::max)();
    arrays->int16_values[2] = (std::numeric_limits<int16_t>::min)();
    arrays->uint16_values[0] = 0;
    arrays->uint16_values[1] = (std::numeric_limits<uint16_t>::max)();
    arrays->uint16_values[2] = 0;
    arrays->int32_values[0] = static_cast<int32_t>(0);
    arrays->int32_values[1] = (std::numeric_limits<int32_t>::max)();
    arrays->int32_values[2] = (std::numeric_limits<int32_t>::min)();
    arrays->uint32_values[0] = 0;
    arrays->uint32_values[1] = (std::numeric_limits<uint32_t>::max)();
    arrays->uint32_values[2] = 0;
    arrays->int64_values[0] = 0;
    arrays->int64_values[1] = (std::numeric_limits<int64_t>::max)();
    arrays->int64_values[2] = (std::numeric_limits<int64_t>::min)();
    arrays->uint64_values[0] = 0;
    arrays->uint64_values[1] = (std::numeric_limits<uint64_t>::max)();
    arrays->uint64_values[2] = 0;
    auto basic_types = get_messages_basic_types_c();
    arrays->basic_types_values[0] = *basic_types[0];
    arrays->basic_types_values[1] = *basic_types[1];
    arrays->basic_types_values[2] = *basic_types[2];
    auto constants = get_messages_constants_c();
    arrays->constants_values[0] = *constants[0];
    arrays->constants_values[1] = *constants[0];
    arrays->constants_values[2] = *constants[0];
    auto defaults = get_messages_defaults_c();
    arrays->defaults_values[0] = *defaults[0];
    arrays->defaults_values[1] = *defaults[0];
    arrays->defaults_values[2] = *defaults[0];
    rosidl_runtime_c__String__assign(&arrays->string_values[0], "");
    rosidl_runtime_c__String__assign(&arrays->string_values[1], "max value");
    rosidl_runtime_c__String__assign(&arrays->string_values[2], "min value");

    vec.push_back(arrays);
  }

  return vec;
}

bool operator==(const test_msgs__msg__Arrays & lhs, const test_msgs__msg__Arrays & rhs)
{
  if (lhs.bool_values[0] != rhs.bool_values[0]) {return false;}
  if (lhs.bool_values[1] != rhs.bool_values[1]) {return false;}
  if (lhs.bool_values[2] != rhs.bool_values[2]) {return false;}
  if (lhs.byte_values[0] != rhs.byte_values[0]) {return false;}
  if (lhs.byte_values[1] != rhs.byte_values[1]) {return false;}
  if (lhs.byte_values[2] != rhs.byte_values[2]) {return false;}
  if (lhs.char_values[0] != rhs.char_values[0]) {return false;}
  if (lhs.char_values[1] != rhs.char_values[1]) {return false;}
  if (lhs.char_values[2] != rhs.char_values[2]) {return false;}
  if (lhs.float32_values[0] != rhs.float32_values[0]) {return false;}
  if (lhs.float32_values[1] != rhs.float32_values[1]) {return false;}
  if (lhs.float32_values[2] != rhs.float32_values[2]) {return false;}
  if (lhs.float64_values[0] != rhs.float64_values[0]) {return false;}
  if (lhs.float64_values[1] != rhs.float64_values[1]) {return false;}
  if (lhs.float64_values[2] != rhs.float64_values[2]) {return false;}
  if (lhs.int8_values[0] != rhs.int8_values[0]) {return false;}
  if (lhs.int8_values[1] != rhs.int8_values[1]) {return false;}
  if (lhs.int8_values[2] != rhs.int8_values[2]) {return false;}
  if (lhs.uint8_values[0] != rhs.uint8_values[0]) {return false;}
  if (lhs.uint8_values[1] != rhs.uint8_values[1]) {return false;}
  if (lhs.uint8_values[2] != rhs.uint8_values[2]) {return false;}
  if (lhs.int16_values[0] != rhs.int16_values[0]) {return false;}
  if (lhs.int16_values[1] != rhs.int16_values[1]) {return false;}
  if (lhs.int16_values[2] != rhs.int16_values[2]) {return false;}
  if (lhs.uint16_values[0] != rhs.uint16_values[0]) {return false;}
  if (lhs.uint16_values[1] != rhs.uint16_values[1]) {return false;}
  if (lhs.uint16_values[2] != rhs.uint16_values[2]) {return false;}
  if (lhs.int32_values[0] != rhs.int32_values[0]) {return false;}
  if (lhs.int32_values[1] != rhs.int32_values[1]) {return false;}
  if (lhs.int32_values[2] != rhs.int32_values[2]) {return false;}
  if (lhs.uint32_values[0] != rhs.uint32_values[0]) {return false;}
  if (lhs.uint32_values[1] != rhs.uint32_values[1]) {return false;}
  if (lhs.uint32_values[2] != rhs.uint32_values[2]) {return false;}
  if (lhs.int64_values[0] != rhs.int64_values[0]) {return false;}
  if (lhs.int64_values[1] != rhs.int64_values[1]) {return false;}
  if (lhs.int64_values[2] != rhs.int64_values[2]) {return false;}
  if (lhs.uint64_values[0] != rhs.uint64_values[0]) {return false;}
  if (lhs.uint64_values[1] != rhs.uint64_values[1]) {return false;}
  if (lhs.uint64_values[2] != rhs.uint64_values[2]) {return false;}
  if (lhs.basic_types_values[0] != rhs.basic_types_values[0]) {return false;}
  if (lhs.basic_types_values[1] != rhs.basic_types_values[1]) {return false;}
  if (lhs.basic_types_values[2] != rhs.basic_types_values[2]) {return false;}
  if (lhs.constants_values[0] != rhs.constants_values[0]) {return false;}
  if (lhs.constants_values[1] != rhs.constants_values[1]) {return false;}
  if (lhs.constants_values[2] != rhs.constants_values[2]) {return false;}
  if (lhs.defaults_values[0] != rhs.defaults_values[0]) {return false;}
  if (lhs.defaults_values[1] != rhs.defaults_values[1]) {return false;}
  if (lhs.defaults_values[2] != rhs.defaults_values[2]) {return false;}

  return true;
}

bool operator!=(const test_msgs__msg__Arrays & lhs, const test_msgs__msg__Arrays & rhs)
{
  return !(lhs == rhs);
}

std::vector<std::shared_ptr<test_msgs__msg__UnboundedSequences>>
get_messages_unbounded_sequences_c()
{
  using T = test_msgs__msg__UnboundedSequences;
  std::vector<std::shared_ptr<T>> vec{};
  {
    auto unbounded = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__UnboundedSequences__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__UnboundedSequences__init(unbounded.get());

    rosidl_runtime_c__bool__Sequence__init(&unbounded->bool_values, 0);
    rosidl_runtime_c__byte__Sequence__init(&unbounded->byte_values, 0);
    rosidl_runtime_c__uint8__Sequence__init(&unbounded->char_values, 0);
    rosidl_runtime_c__float32__Sequence__init(&unbounded->float32_values, 0);
    rosidl_runtime_c__float64__Sequence__init(&unbounded->float64_values, 0);
    rosidl_runtime_c__int8__Sequence__init(&unbounded->int8_values, 0);
    rosidl_runtime_c__uint8__Sequence__init(&unbounded->uint8_values, 0);
    rosidl_runtime_c__int16__Sequence__init(&unbounded->int16_values, 0);
    rosidl_runtime_c__uint16__Sequence__init(&unbounded->uint16_values, 0);
    rosidl_runtime_c__int32__Sequence__init(&unbounded->int32_values, 0);
    rosidl_runtime_c__uint32__Sequence__init(&unbounded->uint32_values, 0);
    rosidl_runtime_c__int64__Sequence__init(&unbounded->int64_values, 0);
    rosidl_runtime_c__uint64__Sequence__init(&unbounded->uint64_values, 0);
    rosidl_runtime_c__String__Sequence__init(&unbounded->string_values, 0);

    unbounded->alignment_check = 0;

    vec.push_back(unbounded);
  }
  {
    auto unbounded = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__UnboundedSequences__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__UnboundedSequences__init(unbounded.get());

    rosidl_runtime_c__bool__Sequence__init(&unbounded->bool_values, 1);
    rosidl_runtime_c__byte__Sequence__init(&unbounded->byte_values, 1);
    rosidl_runtime_c__uint8__Sequence__init(&unbounded->char_values, 1);
    rosidl_runtime_c__float32__Sequence__init(&unbounded->float32_values, 1);
    rosidl_runtime_c__float64__Sequence__init(&unbounded->float64_values, 1);
    rosidl_runtime_c__int8__Sequence__init(&unbounded->int8_values, 1);
    rosidl_runtime_c__uint8__Sequence__init(&unbounded->uint8_values, 1);
    rosidl_runtime_c__int16__Sequence__init(&unbounded->int16_values, 1);
    rosidl_runtime_c__uint16__Sequence__init(&unbounded->uint16_values, 1);
    rosidl_runtime_c__int32__Sequence__init(&unbounded->int32_values, 1);
    rosidl_runtime_c__uint32__Sequence__init(&unbounded->uint32_values, 1);
    rosidl_runtime_c__int64__Sequence__init(&unbounded->int64_values, 1);
    rosidl_runtime_c__uint64__Sequence__init(&unbounded->uint64_values, 1);
    rosidl_runtime_c__String__Sequence__init(&unbounded->string_values, 1);

    unbounded->bool_values.data[0] = true;
    unbounded->byte_values.data[0] = 0xff;
    unbounded->char_values.data[0] = 255;
    unbounded->float32_values.data[0] = 1.125f;
    unbounded->float64_values.data[0] = 1.125;
    unbounded->int8_values.data[0] = (std::numeric_limits<int8_t>::max)();
    unbounded->uint8_values.data[0] = (std::numeric_limits<uint8_t>::max)();
    unbounded->int16_values.data[0] = (std::numeric_limits<int16_t>::max)();
    unbounded->uint16_values.data[0] = (std::numeric_limits<uint16_t>::max)();
    unbounded->int32_values.data[0] = (std::numeric_limits<int32_t>::max)();
    unbounded->uint32_values.data[0] = (std::numeric_limits<uint32_t>::max)();
    unbounded->int64_values.data[0] = (std::numeric_limits<int64_t>::max)();
    unbounded->uint64_values.data[0] = (std::numeric_limits<uint64_t>::max)();
    rosidl_runtime_c__String__assign(&unbounded->string_values.data[0], "max value");
    unbounded->alignment_check = 1;

    vec.push_back(unbounded);
  }
  {
    auto unbounded = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__UnboundedSequences__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__UnboundedSequences__init(unbounded.get());

    rosidl_runtime_c__bool__Sequence__init(&unbounded->bool_values, 2);
    rosidl_runtime_c__byte__Sequence__init(&unbounded->byte_values, 2);
    rosidl_runtime_c__uint8__Sequence__init(&unbounded->char_values, 2);
    rosidl_runtime_c__float32__Sequence__init(&unbounded->float32_values, 3);
    rosidl_runtime_c__float64__Sequence__init(&unbounded->float64_values, 3);
    rosidl_runtime_c__int8__Sequence__init(&unbounded->int8_values, 3);
    rosidl_runtime_c__uint8__Sequence__init(&unbounded->uint8_values, 2);
    rosidl_runtime_c__int16__Sequence__init(&unbounded->int16_values, 3);
    rosidl_runtime_c__uint16__Sequence__init(&unbounded->uint16_values, 2);
    rosidl_runtime_c__int32__Sequence__init(&unbounded->int32_values, 3);
    rosidl_runtime_c__uint32__Sequence__init(&unbounded->uint32_values, 2);
    rosidl_runtime_c__int64__Sequence__init(&unbounded->int64_values, 3);
    rosidl_runtime_c__uint64__Sequence__init(&unbounded->uint64_values, 2);
    rosidl_runtime_c__String__Sequence__init(&unbounded->string_values, 3);

    unbounded->bool_values.data[0] = false;
    unbounded->bool_values.data[1] = true;
    unbounded->byte_values.data[0] = 0x00;
    unbounded->byte_values.data[1] = 0xff;
    unbounded->char_values.data[0] = 0;
    unbounded->char_values.data[1] = 255;
    unbounded->float32_values.data[0] = 0.0f;
    unbounded->float32_values.data[1] = 1.125f;
    unbounded->float32_values.data[2] = -2.125f;
    unbounded->float64_values.data[0] = 0;
    unbounded->float64_values.data[1] = 1.125;
    unbounded->float64_values.data[2] = -2.125;
    unbounded->int8_values.data[0] = 0;
    unbounded->int8_values.data[1] = (std::numeric_limits<int8_t>::max)();
    unbounded->int8_values.data[2] = (std::numeric_limits<int8_t>::min)();
    unbounded->uint8_values.data[0] = 0;
    unbounded->uint8_values.data[1] = (std::numeric_limits<uint8_t>::max)();
    unbounded->int16_values.data[0] = 0;
    unbounded->int16_values.data[1] = (std::numeric_limits<int16_t>::max)();
    unbounded->int16_values.data[2] = (std::numeric_limits<int16_t>::min)();
    unbounded->uint16_values.data[0] = 0;
    unbounded->uint16_values.data[1] = (std::numeric_limits<uint16_t>::max)();
    unbounded->int32_values.data[0] = 0;
    unbounded->int32_values.data[1] = (std::numeric_limits<int32_t>::max)();
    unbounded->int32_values.data[2] = (std::numeric_limits<int32_t>::min)();
    unbounded->uint32_values.data[0] = 0;
    unbounded->uint32_values.data[1] = (std::numeric_limits<uint32_t>::max)();
    unbounded->int64_values.data[0] = 0;
    unbounded->int64_values.data[1] = (std::numeric_limits<int64_t>::max)();
    unbounded->int64_values.data[2] = (std::numeric_limits<int64_t>::min)();
    unbounded->uint64_values.data[0] = 0;
    unbounded->uint64_values.data[1] = (std::numeric_limits<uint64_t>::max)();
    rosidl_runtime_c__String__assign(&unbounded->string_values.data[0], "");
    rosidl_runtime_c__String__assign(&unbounded->string_values.data[1], "max value");
    rosidl_runtime_c__String__assign(&unbounded->string_values.data[2], "optional min value");
    unbounded->alignment_check = 2;

    vec.push_back(unbounded);
  }
  {
    auto unbounded = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__UnboundedSequences__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__UnboundedSequences__init(unbounded.get());

    size_t size = 2000;
    rosidl_runtime_c__bool__Sequence__init(&unbounded->bool_values, size);
    rosidl_runtime_c__byte__Sequence__init(&unbounded->byte_values, size);
    rosidl_runtime_c__uint8__Sequence__init(&unbounded->char_values, size);
    rosidl_runtime_c__float32__Sequence__init(&unbounded->float32_values, size);
    rosidl_runtime_c__float64__Sequence__init(&unbounded->float64_values, size);
    rosidl_runtime_c__int8__Sequence__init(&unbounded->int8_values, size);
    rosidl_runtime_c__uint8__Sequence__init(&unbounded->uint8_values, size);
    rosidl_runtime_c__int16__Sequence__init(&unbounded->int16_values, size);
    rosidl_runtime_c__uint16__Sequence__init(&unbounded->uint16_values, size);
    rosidl_runtime_c__int32__Sequence__init(&unbounded->int32_values, size);
    rosidl_runtime_c__uint32__Sequence__init(&unbounded->uint32_values, size);
    rosidl_runtime_c__int64__Sequence__init(&unbounded->int64_values, size);
    rosidl_runtime_c__uint64__Sequence__init(&unbounded->uint64_values, size);
    rosidl_runtime_c__String__Sequence__init(&unbounded->string_values, size);

    for (size_t i = 0; i < size; ++i) {
      unbounded->bool_values.data[i] = (i % 2 != 0) ? true : false;
      unbounded->byte_values.data[i] = (uint8_t)i;
      unbounded->char_values.data[i] = static_cast<char>(i);
      unbounded->float32_values.data[i] = 1.125f * i;
      unbounded->float64_values.data[i] = 1.125 * i;
      unbounded->int8_values.data[i] = (int8_t)i;
      unbounded->uint8_values.data[i] = (uint8_t)i;
      unbounded->int16_values.data[i] = (int16_t)i;
      unbounded->uint16_values.data[i] = (uint16_t)i;
      unbounded->int32_values.data[i] = (int32_t)i;
      unbounded->uint32_values.data[i] = (uint32_t)i;
      unbounded->int64_values.data[i] = (int64_t)i;
      unbounded->uint64_values.data[i] = (uint64_t)i;
      // Here we use 21 to represent `size` as a string
      // we need 20 characters to represent all size_t values (assuming it's 64bits)
      // +1 character for the null-terminator
      char tmpstr[21];
      snprintf(tmpstr, sizeof(tmpstr), "%zu", i);
      rosidl_runtime_c__String__assign(&unbounded->string_values.data[i], tmpstr);
    }
    unbounded->alignment_check = 3;

    vec.push_back(unbounded);
  }
  {
    auto unbounded = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__UnboundedSequences__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__UnboundedSequences__init(unbounded.get());

    rosidl_runtime_c__bool__Sequence__init(&unbounded->bool_values, 0);
    rosidl_runtime_c__byte__Sequence__init(&unbounded->byte_values, 0);
    rosidl_runtime_c__uint8__Sequence__init(&unbounded->char_values, 0);
    rosidl_runtime_c__float32__Sequence__init(&unbounded->float32_values, 0);
    rosidl_runtime_c__float64__Sequence__init(&unbounded->float64_values, 0);
    rosidl_runtime_c__int8__Sequence__init(&unbounded->int8_values, 0);
    rosidl_runtime_c__uint8__Sequence__init(&unbounded->uint8_values, 0);
    rosidl_runtime_c__int16__Sequence__init(&unbounded->int16_values, 0);
    rosidl_runtime_c__uint16__Sequence__init(&unbounded->uint16_values, 0);
    rosidl_runtime_c__int32__Sequence__init(&unbounded->int32_values, 0);
    rosidl_runtime_c__uint32__Sequence__init(&unbounded->uint32_values, 0);
    rosidl_runtime_c__int64__Sequence__init(&unbounded->int64_values, 0);
    rosidl_runtime_c__uint64__Sequence__init(&unbounded->uint64_values, 0);
    rosidl_runtime_c__String__Sequence__init(&unbounded->string_values, 0);

    unbounded->alignment_check = 4;

    vec.push_back(unbounded);
  }

  return vec;
}

bool operator==(
  const test_msgs__msg__UnboundedSequences & lhs,
  const test_msgs__msg__UnboundedSequences & rhs)
{
  if (lhs.bool_values.size != rhs.bool_values.size) {return false;}
  if (lhs.byte_values.size != rhs.byte_values.size) {return false;}
  if (lhs.char_values.size != rhs.char_values.size) {return false;}
  if (lhs.float32_values.size != rhs.float32_values.size) {return false;}
  if (lhs.float64_values.size != rhs.float64_values.size) {return false;}
  if (lhs.int8_values.size != rhs.int8_values.size) {return false;}
  if (lhs.uint8_values.size != rhs.uint8_values.size) {return false;}
  if (lhs.int16_values.size != rhs.int16_values.size) {return false;}
  if (lhs.uint16_values.size != rhs.uint16_values.size) {return false;}
  if (lhs.int32_values.size != rhs.int32_values.size) {return false;}
  if (lhs.uint32_values.size != rhs.uint32_values.size) {return false;}
  if (lhs.int64_values.size != rhs.int64_values.size) {return false;}
  if (lhs.uint64_values.size != rhs.uint64_values.size) {return false;}

  if (lhs.alignment_check != rhs.alignment_check) {
    fprintf(stderr, "two messages are not aligend\n"); return false;
  }

  for (size_t i = 0; i < lhs.bool_values.size; ++i) {
    if (lhs.bool_values.data[i] != rhs.bool_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.byte_values.size; ++i) {
    if (lhs.byte_values.data[i] != rhs.byte_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.char_values.size; ++i) {
    if (lhs.char_values.data[i] != rhs.char_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.float32_values.size; ++i) {
    if (lhs.float32_values.data[i] != rhs.float32_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.float64_values.size; ++i) {
    if (lhs.float64_values.data[i] != rhs.float64_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.int8_values.size; ++i) {
    if (lhs.int8_values.data[i] != rhs.int8_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.uint8_values.size; ++i) {
    if (lhs.uint8_values.data[i] != rhs.uint8_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.int16_values.size; ++i) {
    if (lhs.int16_values.data[i] != rhs.int16_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.uint16_values.size; ++i) {
    if (lhs.uint16_values.data[i] != rhs.uint16_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.int32_values.size; ++i) {
    if (lhs.int32_values.data[i] != rhs.int32_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.uint32_values.size; ++i) {
    if (lhs.uint32_values.data[i] != rhs.uint32_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.int64_values.size; ++i) {
    if (lhs.int64_values.data[i] != rhs.int64_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.uint64_values.size; ++i) {
    if (lhs.uint64_values.data[i] != rhs.uint64_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.string_values.size; ++i) {
    if (0 != strcmp(lhs.string_values.data[i].data, rhs.string_values.data[i].data)) {return false;}
  }

  return true;
}

bool operator!=(
  const test_msgs__msg__UnboundedSequences & lhs,
  const test_msgs__msg__UnboundedSequences & rhs)
{
  return !(lhs == rhs);
}

std::vector<std::shared_ptr<test_msgs__msg__BoundedSequences>>
get_messages_bounded_sequences_c()
{
  using T = test_msgs__msg__BoundedSequences;
  std::vector<std::shared_ptr<T>> vec{};
  {
    auto bounded = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__BoundedSequences__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__BoundedSequences__init(bounded.get());

    rosidl_runtime_c__bool__Sequence__init(&bounded->bool_values, 3);
    rosidl_runtime_c__byte__Sequence__init(&bounded->byte_values, 3);
    rosidl_runtime_c__uint8__Sequence__init(&bounded->char_values, 3);
    rosidl_runtime_c__float32__Sequence__init(&bounded->float32_values, 3);
    rosidl_runtime_c__float64__Sequence__init(&bounded->float64_values, 3);
    rosidl_runtime_c__int8__Sequence__init(&bounded->int8_values, 3);
    rosidl_runtime_c__uint8__Sequence__init(&bounded->uint8_values, 3);
    rosidl_runtime_c__int16__Sequence__init(&bounded->int16_values, 3);
    rosidl_runtime_c__uint16__Sequence__init(&bounded->uint16_values, 3);
    rosidl_runtime_c__int32__Sequence__init(&bounded->int32_values, 3);
    rosidl_runtime_c__uint32__Sequence__init(&bounded->uint32_values, 3);
    rosidl_runtime_c__int64__Sequence__init(&bounded->int64_values, 3);
    rosidl_runtime_c__uint64__Sequence__init(&bounded->uint64_values, 3);
    rosidl_runtime_c__String__Sequence__init(&bounded->string_values, 3);

    bounded->bool_values.data[0] = false;
    bounded->bool_values.data[1] = true;
    bounded->bool_values.data[2] = false;
    bounded->byte_values.data[0] = 0x00;
    bounded->byte_values.data[1] = 0x01;
    bounded->byte_values.data[2] = 0xff;
    bounded->char_values.data[0] = 0;
    bounded->char_values.data[1] = 1;
    bounded->char_values.data[2] = 255;
    bounded->float32_values.data[0] = 0.0f;
    bounded->float32_values.data[1] = 1.125f;
    bounded->float32_values.data[2] = -2.125f;
    bounded->float64_values.data[0] = 0;
    bounded->float64_values.data[1] = 1.125;
    bounded->float64_values.data[2] = -2.125;
    bounded->int8_values.data[0] = 0;
    bounded->int8_values.data[1] = (std::numeric_limits<int8_t>::max)();
    bounded->int8_values.data[2] = (std::numeric_limits<int8_t>::min)();
    bounded->uint8_values.data[0] = 0;
    bounded->uint8_values.data[1] = 1;
    bounded->uint8_values.data[2] = (std::numeric_limits<uint8_t>::max)();
    bounded->int16_values.data[0] = 0;
    bounded->int16_values.data[1] = (std::numeric_limits<int16_t>::max)();
    bounded->int16_values.data[2] = (std::numeric_limits<int16_t>::min)();
    bounded->uint16_values.data[0] = 0;
    bounded->uint16_values.data[1] = 1;
    bounded->uint16_values.data[2] = (std::numeric_limits<uint16_t>::max)();
    bounded->int32_values.data[0] = 0;
    bounded->int32_values.data[1] = (std::numeric_limits<int32_t>::max)();
    bounded->int32_values.data[2] = (std::numeric_limits<int32_t>::min)();
    bounded->uint32_values.data[0] = 0;
    bounded->uint32_values.data[1] = 1;
    bounded->uint32_values.data[2] = (std::numeric_limits<uint32_t>::max)();
    bounded->int64_values.data[0] = 0;
    bounded->int64_values.data[1] = (std::numeric_limits<int64_t>::max)();
    bounded->int64_values.data[2] = (std::numeric_limits<int64_t>::min)();
    bounded->uint64_values.data[0] = 0;
    bounded->uint64_values.data[1] = 1;
    bounded->uint64_values.data[2] = (std::numeric_limits<uint64_t>::max)();
    rosidl_runtime_c__String__assign(&bounded->string_values.data[0], "");
    rosidl_runtime_c__String__assign(&bounded->string_values.data[1], "max value");
    rosidl_runtime_c__String__assign(&bounded->string_values.data[2], "optional min value");
    bounded->alignment_check = 2;

    vec.push_back(bounded);
  }
  {
    auto bounded = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__BoundedSequences__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__BoundedSequences__init(bounded.get());

    rosidl_runtime_c__bool__Sequence__init(&bounded->bool_values, 0);
    rosidl_runtime_c__byte__Sequence__init(&bounded->byte_values, 0);
    rosidl_runtime_c__uint8__Sequence__init(&bounded->char_values, 0);
    rosidl_runtime_c__float32__Sequence__init(&bounded->float32_values, 0);
    rosidl_runtime_c__float64__Sequence__init(&bounded->float64_values, 0);
    rosidl_runtime_c__int8__Sequence__init(&bounded->int8_values, 0);
    rosidl_runtime_c__uint8__Sequence__init(&bounded->uint8_values, 0);
    rosidl_runtime_c__int16__Sequence__init(&bounded->int16_values, 0);
    rosidl_runtime_c__uint16__Sequence__init(&bounded->uint16_values, 0);
    rosidl_runtime_c__int32__Sequence__init(&bounded->int32_values, 0);
    rosidl_runtime_c__uint32__Sequence__init(&bounded->uint32_values, 0);
    rosidl_runtime_c__int64__Sequence__init(&bounded->int64_values, 0);
    rosidl_runtime_c__uint64__Sequence__init(&bounded->uint64_values, 0);
    rosidl_runtime_c__String__Sequence__init(&bounded->string_values, 0);

    bounded->alignment_check = 4;

    vec.push_back(bounded);
  }

  return vec;
}

bool operator==(
  const test_msgs__msg__BoundedSequences & lhs,
  const test_msgs__msg__BoundedSequences & rhs)
{
  for (size_t i = 0; i < lhs.bool_values.size; ++i) {
    if (lhs.bool_values.data[i] != rhs.bool_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.byte_values.size; ++i) {
    if (lhs.byte_values.data[i] != rhs.byte_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.char_values.size; ++i) {
    if (lhs.char_values.data[i] != rhs.char_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.float32_values.size; ++i) {
    if (lhs.float32_values.data[i] != rhs.float32_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.float64_values.size; ++i) {
    if (lhs.float64_values.data[i] != rhs.float64_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.int8_values.size; ++i) {
    if (lhs.int8_values.data[i] != rhs.int8_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.uint8_values.size; ++i) {
    if (lhs.uint8_values.data[i] != rhs.uint8_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.int16_values.size; ++i) {
    if (lhs.int16_values.data[i] != rhs.int16_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.uint16_values.size; ++i) {
    if (lhs.uint16_values.data[i] != rhs.uint16_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.int32_values.size; ++i) {
    if (lhs.int32_values.data[i] != rhs.int32_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.uint32_values.size; ++i) {
    if (lhs.uint32_values.data[i] != rhs.uint32_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.int64_values.size; ++i) {
    if (lhs.int64_values.data[i] != rhs.int64_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.uint64_values.size; ++i) {
    if (lhs.uint64_values.data[i] != rhs.uint64_values.data[i]) {return false;}
  }
  for (size_t i = 0; i < lhs.string_values.size; ++i) {
    if (0 != strcmp(lhs.string_values.data[i].data, rhs.string_values.data[i].data)) {return false;}
  }

  return true;
}

bool operator!=(
  const test_msgs__msg__BoundedSequences & lhs,
  const test_msgs__msg__BoundedSequences & rhs)
{
  return !(lhs == rhs);
}

std::vector<std::shared_ptr<test_msgs__msg__Nested>>
get_messages_nested_c()
{
  using T = test_msgs__msg__Nested;
  std::vector<std::shared_ptr<T>> vec{};
  {
    auto basic_types = get_messages_basic_types_c();
    for (auto basic_type : basic_types) {
      auto nested = std::shared_ptr<T>(
        new T, [](T * msg) {
          test_msgs__msg__Nested__fini(msg);
          delete msg;
          msg = NULL;
        });
      test_msgs__msg__Nested__init(nested.get());

      test_msgs__msg__BasicTypes__init(&nested->basic_types_value);
      nested->basic_types_value = *basic_type;
      vec.push_back(nested);
    }
  }

  return vec;
}

bool operator==(const test_msgs__msg__Nested & lhs, const test_msgs__msg__Nested & rhs)
{
  return lhs.basic_types_value == rhs.basic_types_value;
}

bool operator!=(const test_msgs__msg__Nested & lhs, const test_msgs__msg__Nested & rhs)
{
  return !(lhs == rhs);
}

/// @todo karsten1987
// This requires some sort of deep-copy operator for arrays and sequences,
// especially their strings as the shared ptr will cleanup the strings
// and eventually lead to memory leaks.
/*
std::vector<std::shared_ptr<test_msgs__msg__MultiNested>>
get_messages_multi_nested_c()
{
  using T = test_msgs__msg__MultiNested;
  std::vector<std::shared_ptr<T>> vec{};
  {
    auto multinested = std::shared_ptr<T>(new T, [](T * msg) {
        test_msgs__msg__MultiNested__fini(msg);
        delete msg;
        msg = NULL;
        });
    test_msgs__msg__MultiNested__init(multinested.get());

    auto arrays_msgs = get_messages_arrays_c();
    auto bounded_sequences_msgs = get_messages_bounded_sequences_c();
    auto unbounded_sequences_msgs = get_messages_unbounded_sequences_c();
    const size_t num_arrays = arrays_msgs.size();
    const size_t num_bounded_sequences = bounded_sequences_msgs.size();
    const size_t num_unbounded_sequences = unbounded_sequences_msgs.size();

    const size_t array_size = 3;
    for (size_t i = 0u; i < array_size; ++i) {
      multinested->array_of_arrays[i] = *arrays_msgs[i % num_arrays];
    }
    for (size_t i = 0u; i < array_size; ++i) {
      multinested->array_of_bounded_sequences[i] =
 *bounded_sequences_msgs[i % num_bounded_sequences];
    }
    for (size_t i = 0u; i < array_size; ++i) {
      multinested->array_of_unbounded_sequences[i] =
 *unbounded_sequences_msgs[i % num_unbounded_sequences];
    }

    const size_t sequence_size = 3;
    test_msgs__msg__Arrays__Sequence__init(&multinested->bounded_sequence_of_arrays, sequence_size);
    for (std::size_t i = 0u; i < sequence_size; ++i) {
      multinested->bounded_sequence_of_arrays.data[i] = *arrays_msgs[i % num_arrays];
    }
    test_msgs__msg__BoundedSequences__Sequence__init(
        &multinested->bounded_sequence_of_bounded_sequences, sequence_size);
    for (std::size_t i = 0u; i < sequence_size; ++i) {
      multinested->bounded_sequence_of_bounded_sequences.data[i] =
        *bounded_sequences_msgs[i % num_bounded_sequences];
    }
    test_msgs__msg__UnboundedSequences__Sequence__init(
        &multinested->bounded_sequence_of_unbounded_sequences, sequence_size);
    for (std::size_t i = 0u; i < sequence_size; ++i) {
      multinested->bounded_sequence_of_unbounded_sequences.data[i] =
        *unbounded_sequences_msgs[i % num_unbounded_sequences];
    }
    test_msgs__msg__Arrays__Sequence__init(
    &multinested->unbounded_sequence_of_arrays, sequence_size);
    for (std::size_t i = 0u; i < sequence_size; ++i) {
      multinested->unbounded_sequence_of_arrays.data[i] = *arrays_msgs[i % num_arrays];
    }
    test_msgs__msg__BoundedSequences__Sequence__init(
        &multinested->unbounded_sequence_of_bounded_sequences, sequence_size);
    for (std::size_t i = 0u; i < sequence_size; ++i) {
      multinested->unbounded_sequence_of_bounded_sequences.data[i] =
        *bounded_sequences_msgs[i % num_bounded_sequences];
    }
    test_msgs__msg__UnboundedSequences__Sequence__init(
        &multinested->unbounded_sequence_of_unbounded_sequences, sequence_size);
    for (std::size_t i = 0u; i < sequence_size; ++i) {
      multinested->unbounded_sequence_of_unbounded_sequences.data[i] =
        *unbounded_sequences_msgs[i % num_unbounded_sequences];
    }

    vec.push_back(multinested);
  }

  return vec;
}

bool operator==(const test_msgs__msg__MultiNested & lhs, const test_msgs__msg__MultiNested & rhs)
{
  const size_t sequence_size = 3u;
  for (size_t i = 0; i < sequence_size; ++i) {
    if (lhs.array_of_arrays[i] != rhs.array_of_arrays[i]) { return false; }
    if (lhs.array_of_bounded_sequences[i] != rhs.array_of_bounded_sequences[i]) { return false; }
    if (lhs.array_of_unbounded_sequences[i] != rhs.array_of_unbounded_sequences[i]) {
      return false;
    }
    if (lhs.bounded_sequence_of_arrays.data[i] != rhs.bounded_sequence_of_arrays.data[i]) {
      return false;
    }
    if (lhs.bounded_sequence_of_bounded_sequences.data[i] !=
        rhs.bounded_sequence_of_bounded_sequences.data[i]) { return false; }
    if (lhs.bounded_sequence_of_unbounded_sequences.data[i] !=
        rhs.bounded_sequence_of_unbounded_sequences.data[i]) { return false; }
    if (lhs.unbounded_sequence_of_arrays.data[i] !=
        rhs.unbounded_sequence_of_arrays.data[i]) { return false; }
    if (lhs.unbounded_sequence_of_bounded_sequences.data[i] !=
        rhs.unbounded_sequence_of_bounded_sequences.data[i]) { return false; }
    if (lhs.unbounded_sequence_of_unbounded_sequences.data[i] !=
        rhs.unbounded_sequence_of_unbounded_sequences.data[i]) { return false; }
  }

  return true;
}

bool operator!=(const test_msgs__msg__MultiNested & lhs, const test_msgs__msg__MultiNested & rhs)
{
  return !(lhs == rhs);
}
*/

std::vector<std::shared_ptr<test_msgs__msg__Builtins>>
get_messages_builtins_c()
{
  using T = test_msgs__msg__Builtins;
  std::vector<std::shared_ptr<T>> vec{};
  {
    auto builtins = std::shared_ptr<T>(
      new T, [](T * msg) {
        test_msgs__msg__Builtins__fini(msg);
        delete msg;
        msg = NULL;
      });
    test_msgs__msg__Builtins__init(builtins.get());
    builtins->duration_value.sec = -1234567890;
    builtins->duration_value.nanosec = 123456789;
    builtins->time_value.sec = -1234567890;
    builtins->time_value.nanosec = 987654321;
    vec.push_back(builtins);
  }

  return vec;
}

bool operator==(const test_msgs__msg__Builtins & lhs, const test_msgs__msg__Builtins & rhs)
{
  if (lhs.duration_value.sec != rhs.duration_value.sec) {return false;}
  if (lhs.duration_value.nanosec != rhs.duration_value.nanosec) {return false;}
  if (lhs.time_value.sec != rhs.time_value.sec) {return false;}
  if (lhs.time_value.nanosec != rhs.time_value.nanosec) {return false;}

  return true;
}

bool operator!=(const test_msgs__msg__Builtins & lhs, const test_msgs__msg__Builtins & rhs)
{
  return !(lhs == rhs);
}

#endif  // TEST_MSGS_C_FIXTURES_HPP_
