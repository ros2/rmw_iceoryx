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
