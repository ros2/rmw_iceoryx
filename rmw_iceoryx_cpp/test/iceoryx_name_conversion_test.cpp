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

#include "rmw_iceoryx_cpp/iceoryx_name_conversion.hpp"

#include <gtest/gtest.h>

#include <string>
#include <tuple>
#include <vector>

TEST(NameConverisonTests, get_name_n_type_from_service_description)
{
  auto topic_and_type = rmw_iceoryx_cpp::get_name_n_type_from_service_description(
    "SERVICE",
    "INSTANCE",
    "EVENT");
  EXPECT_EQ("/INSTANCE/SERVICE/EVENT", std::get<0>(topic_and_type));
  EXPECT_EQ("service_ara_msgs/msg/EVENT", std::get<1>(topic_and_type));
}

TEST(NameConverisonTests, get_service_description_from_name_n_type_revers)
{
  auto service_description = rmw_iceoryx_cpp::get_service_description_from_name_n_type(
    "/INSTANCE/SERVICE/EVENT",
    "service_ara_msgs/msg/EVENT");
  EXPECT_EQ("SERVICE", std::get<0>(service_description));
  EXPECT_EQ("INSTANCE", std::get<1>(service_description));
  EXPECT_EQ("EVENT", std::get<2>(service_description));
}

TEST(NameConverisonTests, get_service_description_from_name_n_type)
{
  auto service_description = rmw_iceoryx_cpp::get_service_description_from_name_n_type(
    "TopicName",
    "TypeName");
  EXPECT_EQ("TypeName", std::get<0>(service_description));
  EXPECT_EQ("TopicName", std::get<1>(service_description));
  EXPECT_EQ("data", std::get<2>(service_description));
}

TEST(NameConverisonTests, get_name_n_type_from_service_description_revers)
{
  auto topic_and_type = rmw_iceoryx_cpp::get_name_n_type_from_service_description(
    "TypeName",
    "TopicName",
    "data");
  EXPECT_EQ("TopicName", std::get<0>(topic_and_type));
  EXPECT_EQ("TypeName", std::get<1>(topic_and_type));
}

TEST(NameConverisonTests, get_hidden_introspection_topic)
{
  auto topic_and_type = rmw_iceoryx_cpp::get_name_n_type_from_service_description(
    "Introspection",
    "RouDI_ID",
    "foo");
  EXPECT_EQ(std::get<0>(topic_and_type), "/_iceoryx/RouDI_ID/Introspection/foo");
  EXPECT_EQ(std::get<1>(topic_and_type), "iceoryx_introspection_msgs/msg/foo");
}

TEST(NameConverisonTests, flip_flop)
{
  std::vector<std::tuple<std::string, std::string>> topic_names_and_types =
  {{"topic1", "type1"},
    {"topic2", "type2"},
    {"topic3", "type3"},
    {"/_iceoryx/RouDI_1/Introspection/event1", "iceoryx_introspection_msgs/msg/event1"},
    {"/_iceoryx/RouDI_1/Introspection/event2", "iceoryx_introspection_msgs/msg/event2"}};

  for (auto & tuple : topic_names_and_types) {
    auto service_tuple =
      rmw_iceoryx_cpp::get_service_description_from_name_n_type(
      std::get<0>(tuple),
      std::get<1>(tuple));
    auto flip_flop_tuple = rmw_iceoryx_cpp::get_name_n_type_from_service_description(
      std::get<0>(service_tuple), std::get<1>(service_tuple), std::get<2>(service_tuple));
    EXPECT_EQ(tuple, flip_flop_tuple);
  }
}

TEST(NameConverisonTests, flip_flop_reverse)
{
  std::vector<std::tuple<std::string, std::string, std::string>> topic_names_and_types =
  {{"service1", "instance1", "event1"},
    {"service2", "instance2", "event2"},
    {"Introspection", "RouDI_1", "event1"},
    {"type1", "topic1", "data"}};

  for (auto & tuple : topic_names_and_types) {
    auto ros_tuple = rmw_iceoryx_cpp::get_name_n_type_from_service_description(
      std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
    auto flip_flop_tuple =
      rmw_iceoryx_cpp::get_service_description_from_name_n_type(
      std::get<0>(ros_tuple),
      std::get<1>(ros_tuple));
    EXPECT_EQ(tuple, flip_flop_tuple);
  }
}
