// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2021 by Apex.AI Inc. All rights reserved.
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

#ifndef RMW_ICEORYX_CPP__ICEORYX_TYPE_INFO_INTROSPECTION_HPP_
#define RMW_ICEORYX_CPP__ICEORYX_TYPE_INFO_INTROSPECTION_HPP_

#include <string>
#include <utility>

struct rosidl_message_type_support_t;

namespace rmw_iceoryx_cpp
{

enum class TypeSupportLanguage
{
  CPP,
  C
};

/// @brief Wraps get_message_typesupport_handle() and does error handling
/// @return std::pair containing enum TypeSupportLanguage and handle to the type support
const std::pair<TypeSupportLanguage, const rosidl_message_type_support_t *> get_type_support(
  const rosidl_message_type_support_t * type_supports);

bool iceoryx_is_fixed_size(const rosidl_message_type_support_t * type_supports);

bool iceoryx_is_valid_type_support(const rosidl_message_type_support_t * type_supports);

size_t iceoryx_get_message_size(const rosidl_message_type_support_t * type_supports);

std::string iceoryx_get_message_name(const rosidl_message_type_support_t * type_supports);

std::string iceoryx_get_message_namespace(const rosidl_message_type_support_t * type_supports);

void iceoryx_init_message(
  const rosidl_message_type_support_t * type_supports,
  void * message);

void iceoryx_fini_message(
  const rosidl_message_type_support_t * type_supports,
  void * message);

}  // namespace rmw_iceoryx_cpp
#endif  // RMW_ICEORYX_CPP__ICEORYX_TYPE_INFO_INTROSPECTION_HPP_
