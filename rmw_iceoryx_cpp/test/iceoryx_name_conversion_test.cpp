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

TEST(NameConverisonTests, get_hidden_topic)
{
  auto topic_and_type = rmw_iceoryx_cpp::get_name_n_type_from_service_description(
    "Introspection",
    "RouDI_ID",
    "foo");
  EXPECT_EQ(std::get<0>(topic_and_type), "/_iceoryx/RouDI_ID/Introspection/foo");
  EXPECT_EQ(std::get<1>(topic_and_type), "iceoryx_introspection_msgs/msg/foo");
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
