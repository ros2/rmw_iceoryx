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

#include "rmw_iceoryx_cpp/iceoryx_deserialize.hpp"
#include "rmw_iceoryx_cpp/iceoryx_serialize.hpp"
#include "rmw_iceoryx_cpp/iceoryx_type_info_introspection.hpp"

#include "rosidl_typesupport_introspection_c/identifier.h"

#include "rosidl_typesupport_introspection_cpp/identifier.hpp"

namespace details
{
rmw_ret_t
copy_payload_into_serialized_message(
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
    return details::copy_payload_into_serialized_message(
      serialized_message, ros_message, rmw_iceoryx_cpp::iceoryx_get_message_size(type_supports));
  }

  // it's no fixed size message, so we have to serialize
  std::vector<char> payload_vector{};
  rmw_iceoryx_cpp::serialize(ros_message, type_supports, payload_vector);

  return details::copy_payload_into_serialized_message(
    serialized_message, payload_vector.data(), payload_vector.size());
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

  // it's a fixed size message, so we memcpy
  if (rmw_iceoryx_cpp::iceoryx_is_fixed_size(type_supports)) {
    memcpy(ros_message, serialized_message->buffer, serialized_message->buffer_length);
    return RMW_RET_OK;
  }

  rmw_iceoryx_cpp::deserialize(
    reinterpret_cast<const char *>(serialized_message->buffer), type_supports, ros_message);

  return RMW_RET_OK;
}

rmw_ret_t
rmw_get_serialized_message_size(
  const rosidl_message_type_support_t * type_supports,
  const rosidl_runtime_c__Sequence__bound * message_bounds,
  size_t * size)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(type_supports, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(message_bounds, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(size, RMW_RET_ERROR);

  *size = rmw_iceoryx_cpp::iceoryx_get_message_size(type_supports);

  return RMW_RET_OK;
}
}  // extern "C"
