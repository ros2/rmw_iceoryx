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

#include "rcutils/error_handling.h"
#include "rmw/event.h"
#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

extern "C"
{
rmw_ret_t
rmw_publisher_event_init(
  rmw_event_t * rmw_event,
  const rmw_publisher_t * publisher,
  rmw_event_type_t event_type)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(rmw_event, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_ERROR);
  (void) event_type;

  /// @todo add publisher events support
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_subscription_event_init(
  rmw_event_t * rmw_event,
  const rmw_subscription_t * subscription,
  rmw_event_type_t event_type)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(rmw_event, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_ERROR);
  (void) event_type;

  /// @todo add subscription events support
  return RMW_RET_UNSUPPORTED;
}
}  // extern "C"
