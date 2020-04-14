// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
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

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

#include "rosidl_typesupport_cpp/message_type_support.hpp"

#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"

#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"

#include "rmw_iceoryx_cpp/iceoryx_type_info_introspection.hpp"

namespace details_cpp
{
bool is_complex_type(const rosidl_typesupport_introspection_cpp::MessageMember * member)
{
  return member->type_id_ == ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE;
}

bool is_vector_type(const rosidl_typesupport_introspection_cpp::MessageMember * member)
{
  return member->type_id_ == ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING ||
         (member->is_array_ && !(member->array_size_ > 0 && !member->is_upper_bound_));
}

bool is_fixed_size(const rosidl_typesupport_introspection_cpp::MessageMembers * members)
{
  static auto cache = std::unordered_map<std::string, bool>();

  std::string name = members->message_namespace_;
  name += members->message_name_;

  auto search = cache.find(name);
  if (search != cache.end()) {
    return search->second;
  }

  for (uint32_t i = 0; i < members->member_count_; ++i) {
    const auto * member = members->members_ + i;
    // if ROS type, call recursively
    if (is_complex_type(member)) {
      auto result = is_fixed_size(
        (const rosidl_typesupport_introspection_cpp::MessageMembers *)member->members_->data);
      cache.insert({name, result});
      // early exit
      if (!result) {
        return false;
      }
    } else if (is_vector_type(member)) {
      cache.insert({name, false});
      return false;
    } else {
      cache.insert({name, true});
    }
  }

  return true;
}
}  // namespace details_cpp

namespace details_c
{
bool is_complex_type(const rosidl_typesupport_introspection_c__MessageMember * member)
{
  return member->type_id_ == ::rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE;
}

bool is_vector_type(const rosidl_typesupport_introspection_c__MessageMember * member)
{
  return member->type_id_ == ::rosidl_typesupport_introspection_c__ROS_TYPE_STRING ||
         (member->is_array_ && !(member->array_size_ > 0 && !member->is_upper_bound_));
}

bool is_fixed_size(const rosidl_typesupport_introspection_c__MessageMembers * members)
{
  static auto cache = std::unordered_map<std::string, bool>();

  std::string name = members->message_namespace_;
  name += members->message_name_;

  auto search = cache.find(name);
  if (search != cache.end()) {
    return search->second;
  }

  for (uint32_t i = 0; i < members->member_count_; ++i) {
    const auto * member = members->members_ + i;
    // if ROS type, call recursively
    if (is_complex_type(member)) {
      auto result = is_fixed_size(
        (const rosidl_typesupport_introspection_c__MessageMembers *)member->members_->data);
      cache.insert({name, result});
      // early exit
      if (!result) {
        return false;
      }
    } else if (is_vector_type(member)) {
      cache.insert({name, false});
      return false;
    } else {
      cache.insert({name, true});
    }
  }

  return true;
}
}  // namespace details_c

namespace rmw_iceoryx_cpp
{

bool iceoryx_is_fixed_size(const rosidl_message_type_support_t * type_supports)
{
  // first, try to extract cpp type support
  auto ts_cpp = get_message_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_cpp::typesupport_identifier);
  if (ts_cpp != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(ts_cpp->data);
    return details_cpp::is_fixed_size(members);
  }

  // second, try to extract c type support
  auto ts_c = get_message_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_c__identifier);
  if (ts_c != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(ts_c->data);
    return details_c::is_fixed_size(members);
  }

  // still here? Then something's wrong
  throw std::runtime_error("no suitable type support given");
}

size_t iceoryx_get_message_size(const rosidl_message_type_support_t * type_supports)
{
  // first, try to extract cpp type support
  auto ts_cpp = get_message_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_cpp::typesupport_identifier);
  if (ts_cpp != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(ts_cpp->data);
    return members->size_of_;
  }

  // second, try to extract c type support
  auto ts_c = get_message_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_c__identifier);
  if (ts_c != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(ts_c->data);
    return members->size_of_;
  }

  // still here? Then something's wrong
  throw std::runtime_error("no suitable type support given");
}

std::string iceoryx_get_message_name(const rosidl_message_type_support_t * type_supports)
{
  // first, try to extract cpp type support
  auto ts_cpp = get_message_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_cpp::typesupport_identifier);
  if (ts_cpp != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(ts_cpp->data);
    return members->message_name_;
  }

  // second, try to extract c type support
  auto ts_c = get_message_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_c__identifier);
  if (ts_c != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(ts_c->data);
    return members->message_name_;
  }

  // still here? Then something's wrong
  throw std::runtime_error("no suitable type support given");
}

std::string iceoryx_get_message_namespace(const rosidl_message_type_support_t * type_supports)
{
  // first, try to extract cpp type support
  auto ts_cpp = get_message_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_cpp::typesupport_identifier);
  if (ts_cpp != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(ts_cpp->data);
    return members->message_namespace_;
  }

  // second, try to extract c type support
  auto ts_c = get_message_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_c__identifier);
  if (ts_c != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(ts_c->data);
    return members->message_namespace_;
  }

  // still here? Then something's wrong
  throw std::runtime_error("no suitable type support given");
}

bool iceoryx_is_valid_type_support(const rosidl_message_type_support_t * type_supports)
{
  if (get_message_typesupport_handle(
      type_supports, rosidl_typesupport_introspection_cpp::typesupport_identifier))
  {
    return true;
  }
  if (get_message_typesupport_handle(
      type_supports, rosidl_typesupport_introspection_c__identifier))
  {
    return true;
  }
  return false;
}

void iceoryx_init_message(
  const rosidl_message_type_support_t * type_supports,
  void * message)
{
  // first, try to extract cpp type support
  auto ts_cpp = get_message_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_cpp::typesupport_identifier);
  if (ts_cpp != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(ts_cpp->data);
    members->init_function(message, rosidl_runtime_cpp::MessageInitialization::ALL);
    return;
  }

  // second, try to extract c type support
  auto ts_c = get_message_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_c__identifier);
  if (ts_c != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(ts_c->data);
    members->init_function(message, ROSIDL_RUNTIME_C_MSG_INIT_ALL);
    return;
  }

  // still here? Then something's wrong
  throw std::runtime_error("no suitable type support given");
}

void iceoryx_fini_message(
  const rosidl_message_type_support_t * type_supports,
  void * message)
{
  // first, try to extract cpp type support
  auto ts_cpp = get_message_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_cpp::typesupport_identifier);
  if (ts_cpp != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(ts_cpp->data);
    members->fini_function(message);
    return;
  }

  // second, try to extract c type support
  auto ts_c = get_message_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_c__identifier);
  if (ts_c != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(ts_c->data);
    members->fini_function(message);
    return;
  }

  // still here? Then something's wrong
  throw std::runtime_error("no suitable type support given");
}

}  // namespace rmw_iceoryx_cpp
