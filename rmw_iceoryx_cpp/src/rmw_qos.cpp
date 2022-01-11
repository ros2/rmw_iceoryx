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

#include "rmw/qos_profiles.h"
#include "rcutils/error_handling.h"
#include "rmw/impl/cpp/macros.hpp"

extern "C"
{
rmw_ret_t
rmw_qos_profile_check_compatible(
  const rmw_qos_profile_t publisher_profile,
  const rmw_qos_profile_t subscription_profile,
  rmw_qos_compatibility_type_t * compatibility,
  char * reason,
  size_t reason_size)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(compatibility, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(reason, RMW_RET_INVALID_ARGUMENT);
  (void)publisher_profile;
  (void)subscription_profile;
  (void)reason_size;

  // As iceoryx does not consider QoS for matching, always set ok
  *compatibility = RMW_QOS_COMPATIBILITY_OK;
  // Un-terminated char array leads to crashes in rqt_graph
  reason[0] = '\0';
  return RMW_RET_OK;
}
}  // extern "C"
