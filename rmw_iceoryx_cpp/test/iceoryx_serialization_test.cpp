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

#include "rmw_iceoryx_cpp/iceoryx_deserialize.hpp"
#include "rmw_iceoryx_cpp/iceoryx_serialize.hpp"

#include <gtest/gtest.h>

#include <typeinfo>
#include <vector>

#include "rosidl_typesupport_cpp/message_type_support.hpp"

#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"

#include "test_msgs/message_fixtures.hpp"

template<class MessageT>
std::string to_string(const MessageT & msg)
{
  (void) msg;
  return std::string("no to_string method defined") + typeid(MessageT).name();
}

template<>
std::string to_string<test_msgs::msg::Strings>(const test_msgs::msg::Strings & msg)
{
  std::string str{};
  str += "string_value: " + msg.string_value + "\n";
  str += "bounded_string_value: " + msg.bounded_string_value + "\n";
  return str;
}

template<class MessageT>
void test_equality(const MessageT & expected, const MessageT & actual)
{
  EXPECT_EQ(expected, actual)
    << "expected: " << to_string(expected)
    << "got: " << to_string(actual);
}

template<>
void test_equality(const test_msgs::msg::Strings & expected, const test_msgs::msg::Strings & actual)
{
  EXPECT_STREQ(expected.string_value.c_str(), actual.string_value.c_str());
  EXPECT_STREQ(expected.bounded_string_value.c_str(), actual.bounded_string_value.c_str());
}

template<>
void test_equality(const test_msgs::msg::Arrays & expected, const test_msgs::msg::Arrays & actual)
{
  EXPECT_EQ(expected.bool_values, actual.bool_values);
  EXPECT_EQ(expected.byte_values, actual.byte_values);
  EXPECT_EQ(expected.char_values, actual.char_values);
  EXPECT_EQ(expected.float32_values, actual.float32_values);
  EXPECT_EQ(expected.float64_values, actual.float64_values);
  EXPECT_EQ(expected.int8_values, actual.int8_values);
  EXPECT_EQ(expected.uint8_values, actual.uint8_values);
  EXPECT_EQ(expected.int16_values, actual.int16_values);
  EXPECT_EQ(expected.uint16_values, actual.uint16_values);
  EXPECT_EQ(expected.int32_values, actual.int32_values);
  EXPECT_EQ(expected.uint32_values, actual.uint32_values);
  EXPECT_EQ(expected.int64_values, actual.int64_values);
  EXPECT_EQ(expected.uint64_values, actual.uint64_values);
  EXPECT_EQ(expected.string_values, actual.string_values);
  EXPECT_EQ(expected.basic_types_values, actual.basic_types_values);
  EXPECT_EQ(expected.constants_values, actual.constants_values);
  EXPECT_EQ(expected.defaults_values, actual.defaults_values);
  EXPECT_EQ(expected.bool_values_default, actual.bool_values_default);
  EXPECT_EQ(expected.byte_values_default, actual.byte_values_default);
  EXPECT_EQ(expected.char_values_default, actual.char_values_default);
  EXPECT_EQ(expected.float32_values_default, actual.float32_values_default);
  EXPECT_EQ(expected.float64_values_default, actual.float64_values_default);
  EXPECT_EQ(expected.int8_values_default, actual.int8_values_default);
  EXPECT_EQ(expected.uint8_values_default, actual.uint8_values_default);
  EXPECT_EQ(expected.int16_values_default, actual.int16_values_default);
  EXPECT_EQ(expected.uint16_values_default, actual.uint16_values_default);
  EXPECT_EQ(expected.int32_values_default, actual.int32_values_default);
  EXPECT_EQ(expected.uint32_values_default, actual.uint32_values_default);
  EXPECT_EQ(expected.int64_values_default, actual.int64_values_default);
  EXPECT_EQ(expected.uint64_values_default, actual.uint64_values_default);
  EXPECT_EQ(expected.string_values_default, actual.string_values_default);
  EXPECT_EQ(expected.alignment_check, actual.alignment_check);
}

template<>
void test_equality(const test_msgs::msg::UnboundedSequences & expected, const test_msgs::msg::UnboundedSequences & actual)
{
  EXPECT_EQ(expected.bool_values, actual.bool_values);
  EXPECT_EQ(expected.byte_values, actual.byte_values);
  EXPECT_EQ(expected.char_values, actual.char_values);
  EXPECT_EQ(expected.float32_values, actual.float32_values);
  EXPECT_EQ(expected.float64_values, actual.float64_values);
  EXPECT_EQ(expected.int8_values, actual.int8_values);
  EXPECT_EQ(expected.uint8_values, actual.uint8_values);
  EXPECT_EQ(expected.int16_values, actual.int16_values);
  EXPECT_EQ(expected.uint16_values, actual.uint16_values);
  EXPECT_EQ(expected.int32_values, actual.int32_values);
  EXPECT_EQ(expected.uint32_values, actual.uint32_values);
  EXPECT_EQ(expected.int64_values, actual.int64_values);
  EXPECT_EQ(expected.uint64_values, actual.uint64_values);
  EXPECT_EQ(expected.string_values, actual.string_values);
  EXPECT_EQ(expected.basic_types_values, actual.basic_types_values);
  EXPECT_EQ(expected.constants_values, actual.constants_values);
  EXPECT_EQ(expected.defaults_values, actual.defaults_values);
  EXPECT_EQ(expected.bool_values_default, actual.bool_values_default);
  EXPECT_EQ(expected.byte_values_default, actual.byte_values_default);
  EXPECT_EQ(expected.char_values_default, actual.char_values_default);
  EXPECT_EQ(expected.float32_values_default, actual.float32_values_default);
  EXPECT_EQ(expected.float64_values_default, actual.float64_values_default);
  EXPECT_EQ(expected.int8_values_default, actual.int8_values_default);
  EXPECT_EQ(expected.uint8_values_default, actual.uint8_values_default);
  EXPECT_EQ(expected.int16_values_default, actual.int16_values_default);
  EXPECT_EQ(expected.uint16_values_default, actual.uint16_values_default);
  EXPECT_EQ(expected.int32_values_default, actual.int32_values_default);
  EXPECT_EQ(expected.uint32_values_default, actual.uint32_values_default);
  EXPECT_EQ(expected.int64_values_default, actual.int64_values_default);
  EXPECT_EQ(expected.uint64_values_default, actual.uint64_values_default);
  EXPECT_EQ(expected.string_values_default, actual.string_values_default);
  EXPECT_EQ(expected.alignment_check, actual.alignment_check);
}

template<>
void test_equality(const test_msgs::msg::BoundedSequences & expected, const test_msgs::msg::BoundedSequences & actual)
{
  EXPECT_EQ(expected.bool_values, actual.bool_values);
  EXPECT_EQ(expected.byte_values, actual.byte_values);
  EXPECT_EQ(expected.char_values, actual.char_values);
  EXPECT_EQ(expected.float32_values, actual.float32_values);
  EXPECT_EQ(expected.float64_values, actual.float64_values);
  EXPECT_EQ(expected.int8_values, actual.int8_values);
  EXPECT_EQ(expected.uint8_values, actual.uint8_values);
  EXPECT_EQ(expected.int16_values, actual.int16_values);
  EXPECT_EQ(expected.uint16_values, actual.uint16_values);
  EXPECT_EQ(expected.int32_values, actual.int32_values);
  EXPECT_EQ(expected.uint32_values, actual.uint32_values);
  EXPECT_EQ(expected.int64_values, actual.int64_values);
  EXPECT_EQ(expected.uint64_values, actual.uint64_values);
  EXPECT_EQ(expected.string_values, actual.string_values);
  EXPECT_EQ(expected.basic_types_values, actual.basic_types_values);
  EXPECT_EQ(expected.constants_values, actual.constants_values);
  EXPECT_EQ(expected.defaults_values, actual.defaults_values);
  EXPECT_EQ(expected.bool_values_default, actual.bool_values_default);
  EXPECT_EQ(expected.byte_values_default, actual.byte_values_default);
  EXPECT_EQ(expected.char_values_default, actual.char_values_default);
  EXPECT_EQ(expected.float32_values_default, actual.float32_values_default);
  EXPECT_EQ(expected.float64_values_default, actual.float64_values_default);
  EXPECT_EQ(expected.int8_values_default, actual.int8_values_default);
  EXPECT_EQ(expected.uint8_values_default, actual.uint8_values_default);
  EXPECT_EQ(expected.int16_values_default, actual.int16_values_default);
  EXPECT_EQ(expected.uint16_values_default, actual.uint16_values_default);
  EXPECT_EQ(expected.int32_values_default, actual.int32_values_default);
  EXPECT_EQ(expected.uint32_values_default, actual.uint32_values_default);
  EXPECT_EQ(expected.int64_values_default, actual.int64_values_default);
  EXPECT_EQ(expected.uint64_values_default, actual.uint64_values_default);
  EXPECT_EQ(expected.string_values_default, actual.string_values_default);
  EXPECT_EQ(expected.alignment_check, actual.alignment_check);
}

template<
  class MessageT,
  class MessageFixtureF = std::function<std::vector<std::shared_ptr<MessageT>>(void)>
>
void flip_flop_serialization(MessageFixtureF message_fixture)
{
  auto ts = rosidl_typesupport_cpp::get_message_type_support_handle<MessageT>();

  auto test_msgs = message_fixture();
  for (auto i = 0u; i < test_msgs.size(); ++i) {
  //for (auto i = 0u; i < 1; ++i) {
    fprintf(stderr, "+++ Message #%u +++\n", i);
    auto test_msg = test_msgs[i];
    std::vector<char> payload{};

    MessageT * msg = test_msg.get();
    rmw_iceoryx_cpp::serialize(msg, ts, payload);
    ASSERT_FALSE(payload.empty());

    MessageT deserialized_msg{};
    rmw_iceoryx_cpp::deserialize(payload.data(), ts, &deserialized_msg);

    test_equality<MessageT>(*msg, deserialized_msg);
  }
}

TEST(SerializationTests, flip_flop_serialize_basic_types)
{
  flip_flop_serialization<test_msgs::msg::Empty>(std::bind(&get_messages_empty));
  flip_flop_serialization<test_msgs::msg::BasicTypes>(std::bind(&get_messages_basic_types));
  flip_flop_serialization<test_msgs::msg::Constants>(std::bind(&get_messages_constants));
  flip_flop_serialization<test_msgs::msg::Defaults>(std::bind(&get_messages_defaults));
  flip_flop_serialization<test_msgs::msg::Strings>(std::bind(&get_messages_strings));
  flip_flop_serialization<test_msgs::msg::Arrays>(std::bind(&get_messages_arrays));
  flip_flop_serialization<test_msgs::msg::UnboundedSequences>(std::bind(&get_messages_unbounded_sequences));
  flip_flop_serialization<test_msgs::msg::BoundedSequences>(std::bind(&get_messages_bounded_sequences));
  flip_flop_serialization<test_msgs::msg::MultiNested>(std::bind(&get_messages_multi_nested));
  flip_flop_serialization<test_msgs::msg::Nested>(std::bind(&get_messages_nested));
  flip_flop_serialization<test_msgs::msg::Builtins>(std::bind(&get_messages_builtins));
  flip_flop_serialization<test_msgs::msg::WStrings>(std::bind(&get_messages_wstrings));
}
