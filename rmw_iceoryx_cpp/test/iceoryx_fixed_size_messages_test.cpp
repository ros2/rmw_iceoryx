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

#include "rmw_iceoryx_cpp/iceoryx_type_info_introspection.hpp"

#include <gtest/gtest.h>

#include "rosidl_typesupport_cpp/message_type_support.hpp"

#include "test_msgs/message_fixtures.hpp"

#include "./test_msgs_c_fixtures.hpp"

bool is_fixed_size(const rosidl_message_type_support_t * ts)
{
  return rmw_iceoryx_cpp::iceoryx_is_fixed_size(ts);
}

template<class MessageT>
bool is_fixed_size()
{
  auto ts = rosidl_typesupport_cpp::get_message_type_support_handle<MessageT>();

  return is_fixed_size(ts);
}

TEST(FixedSizeMessagesTest, test_primitives)
{
  EXPECT_TRUE(is_fixed_size<test_msgs::msg::Empty>());
  EXPECT_TRUE(is_fixed_size<test_msgs::msg::BasicTypes>());
  EXPECT_TRUE(is_fixed_size<test_msgs::msg::Builtins>());
  EXPECT_TRUE(is_fixed_size<test_msgs::msg::Defaults>());
  EXPECT_TRUE(is_fixed_size<test_msgs::msg::Nested>());
}

TEST(FixedSizeMessagesTest, test_complex_types)
{
  EXPECT_FALSE(is_fixed_size<test_msgs::msg::Strings>());
  EXPECT_FALSE(is_fixed_size<test_msgs::msg::WStrings>());
  EXPECT_FALSE(is_fixed_size<test_msgs::msg::Arrays>());
  EXPECT_FALSE(is_fixed_size<test_msgs::msg::BoundedSequences>());
  EXPECT_FALSE(is_fixed_size<test_msgs::msg::UnboundedSequences>());
  EXPECT_FALSE(is_fixed_size<test_msgs::msg::MultiNested>());
}

TEST(FixedSizeMessagesTest, test_cache_behavior)
{
  EXPECT_FALSE(is_fixed_size<test_msgs::msg::MultiNested>());

  EXPECT_FALSE(is_fixed_size<test_msgs::msg::Strings>());
  EXPECT_FALSE(is_fixed_size<test_msgs::msg::WStrings>());
  EXPECT_FALSE(is_fixed_size<test_msgs::msg::Arrays>());
  EXPECT_FALSE(is_fixed_size<test_msgs::msg::BoundedSequences>());
  EXPECT_FALSE(is_fixed_size<test_msgs::msg::UnboundedSequences>());
}

TEST(FixedSizeMessagesTest, test_primitives_c)
{
  auto ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Empty);
  EXPECT_TRUE(is_fixed_size(ts));
  ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  EXPECT_TRUE(is_fixed_size(ts));
  ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Builtins);
  EXPECT_TRUE(is_fixed_size(ts));
  ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Defaults);
  EXPECT_TRUE(is_fixed_size(ts));
  ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Nested);
  EXPECT_TRUE(is_fixed_size(ts));
}

TEST(FixedSizeMessagesTest, test_complex_types_c)
{
  auto ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  EXPECT_FALSE(is_fixed_size(ts));
  ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Arrays);
  EXPECT_FALSE(is_fixed_size(ts));
  ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BoundedSequences);
  EXPECT_FALSE(is_fixed_size(ts));
  ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, UnboundedSequences);
  EXPECT_FALSE(is_fixed_size(ts));
  ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, MultiNested);
  EXPECT_FALSE(is_fixed_size(ts));
}

TEST(FixedSizeMessagesTest, test_cache_behavior_c)
{
  auto ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, MultiNested);
  EXPECT_FALSE(is_fixed_size(ts));

  ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  EXPECT_FALSE(is_fixed_size(ts));
  ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Arrays);
  EXPECT_FALSE(is_fixed_size(ts));
  ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BoundedSequences);
  EXPECT_FALSE(is_fixed_size(ts));
  ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, UnboundedSequences);
  EXPECT_FALSE(is_fixed_size(ts));
  ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, MultiNested);
  EXPECT_FALSE(is_fixed_size(ts));
}
