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

TEST(NameConverisonTests, get_name_n_type_from_iceoryx_service_description)
{
  auto topic_and_type = rmw_iceoryx_cpp::get_name_n_type_from_iceoryx_service_description(
    "SERVICE",
    "INSTANCE",
    "EVENT");
  EXPECT_EQ(std::get<0>(topic_and_type), "/INSTANCE/SERVICE/EVENT");
  EXPECT_EQ(std::get<1>(topic_and_type), "service_ara_msgs/msg/EVENT");
}

TEST(NameConverisonTests, get_service_description_elements_revers)
{
  auto service_description = rmw_iceoryx_cpp::get_service_description_elements(
    "/INSTANCE/SERVICE/EVENT",
    "service_ara_msgs/msg/EVENT");
  EXPECT_EQ(std::get<0>(service_description), "SERVICE");
  EXPECT_EQ(std::get<1>(service_description), "INSTANCE");
  EXPECT_EQ(std::get<2>(service_description), "EVENT");
}

TEST(NameConverisonTests, get_service_description_elements)
{
  auto service_description = rmw_iceoryx_cpp::get_service_description_elements(
    "TopicName",
    "TypeName");
  EXPECT_EQ(std::get<0>(service_description), "TypeName");
  EXPECT_EQ(std::get<1>(service_description), "TopicName");
  EXPECT_EQ(std::get<2>(service_description), "data");
}

TEST(NameConverisonTests, get_name_n_type_from_iceoryx_service_description_revers)
{
  auto topic_and_type = rmw_iceoryx_cpp::get_name_n_type_from_iceoryx_service_description(
    "TypeName",
    "TopicName",
    "data");
  EXPECT_EQ(std::get<0>(topic_and_type), "TopicName");
  EXPECT_EQ(std::get<1>(topic_and_type), "TypeName");
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
