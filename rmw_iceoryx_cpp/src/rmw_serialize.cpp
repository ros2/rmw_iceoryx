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

#include <vector>

#include "rcutils/error_handling.h"

#include "rmw/rmw.h"

#include "rmw_iceoryx_cpp/iceoryx_serialize.hpp"
#include "rmw_iceoryx_cpp/iceoryx_type_info_introspection.hpp"

#include "rosidl_typesupport_introspection_c/identifier.h"

#include "rosidl_typesupport_introspection_cpp/identifier.hpp"

namespace details
{
rmw_ret_t
copy_payload(
  rmw_serialized_message_t * serialized_message,
  const void * payload,
  size_t payload_size)
{
  auto ret = rmw_serialized_message_resize(serialized_message, payload_size);
  if (RMW_RET_OK == ret) {
    memcpy(serialized_message->buffer, payload, payload_size);
    serialized_message->buffer_length = payload_size;
  }
  return ret;
}
}  // namespace details

extern "C"
{
rmw_ret_t
rmw_serialize(
  const void * ros_message,
  const rosidl_message_type_support_t * type_supports,
  rmw_serialized_message_t * serialized_message)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(ros_message, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(type_supports, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(serialized_message, RMW_RET_ERROR);

  // it's a fixed size message, so we memcpy
  if (rmw_iceoryx_cpp::iceoryx_is_fixed_size(type_supports)) {
    return details::copy_payload(
      serialized_message, ros_message, rmw_iceoryx_cpp::iceoryx_get_message_size(type_supports));
  }

  // message is neither loaned nor fixed size, so we have to serialize
  std::vector<char> payload_vector{};

  // serialize with cpp typesupport
  auto ts_cpp = get_message_typesupport_handle(
    type_supports,
    rosidl_typesupport_introspection_cpp::typesupport_identifier);
  if (ts_cpp != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(ts_cpp->data);
    rmw_iceoryx_cpp::serialize(ros_message, members, payload_vector);
  }

  // serialize with c typesupport
  auto ts_c = get_message_typesupport_handle(
    type_supports,
    rosidl_typesupport_introspection_c__identifier);
  if (ts_c != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(ts_c->data);
    rmw_iceoryx_cpp::serialize(ros_message, members, payload_vector);
  }

  return details::copy_payload(serialized_message, payload_vector.data(), payload_vector.size());
}

rmw_ret_t
rmw_deserialize(
  const rmw_serialized_message_t * serialized_message,
  const rosidl_message_type_support_t * type_supports,
  void * ros_message)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(serialized_message, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(type_supports, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(ros_message, RMW_RET_ERROR);

  assert(false);
  return RMW_RET_OK;
}

rmw_ret_t
rmw_get_serialized_message_size(
  const rosidl_message_type_support_t * type_supports,
  const rosidl_message_bounds_t * message_bounds,
  size_t * size)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(type_supports, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(message_bounds, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(size, RMW_RET_ERROR);

  *size = rmw_iceoryx_cpp::iceoryx_get_message_size(type_supports);

  return RMW_RET_OK;
}
}  // extern "C"
