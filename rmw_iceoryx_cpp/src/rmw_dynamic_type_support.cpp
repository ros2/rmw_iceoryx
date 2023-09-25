// Copyright (c) 2023 by Apex.AI Inc. All rights reserved.
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

#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

extern "C"
{
rmw_ret_t
rmw_take_dynamic_message(
  const rmw_subscription_t * subscription,
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(dynamic_message, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(allocation, RMW_RET_ERROR);

  RMW_SET_ERROR_MSG("rmw_take_dynamic_message is not supported in iceoryx");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_take_dynamic_message_with_info(
  const rmw_subscription_t * subscription,
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(dynamic_message, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(message_info, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(allocation, RMW_RET_ERROR);

  RMW_SET_ERROR_MSG("rmw_take_dynamic_message_with_info is not supported in iceoryx");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_serialization_support_init(
  const char * serialization_lib_name,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_serialization_support_t * serialization_support)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(serialization_lib_name, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(allocator, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(serialization_support, RMW_RET_ERROR);

  RMW_SET_ERROR_MSG("rmw_serialization_support_init is not supported in iceoryx");
  return RMW_RET_UNSUPPORTED;
}
}  // extern "C"
