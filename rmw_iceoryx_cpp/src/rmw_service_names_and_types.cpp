// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
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

#include "rcutils/error_handling.h"

#include "rmw/get_service_names_and_types.h"
#include "rmw/impl/cpp/macros.hpp"
#include "rmw/names_and_types.h"
#include "rmw/rmw.h"

#include "rmw_iceoryx_cpp/iceoryx_topic_names_and_types.hpp"

extern "C"
{
rmw_ret_t
rmw_get_service_names_and_types(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  rmw_names_and_types_t * service_names_and_types)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(allocator, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(service_names_and_types, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_get_service_names_and_types
    : node, node->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  rmw_ret_t rmw_ret = rmw_names_and_types_check_zero(service_names_and_types);
  if (rmw_ret != RMW_RET_OK) {
    return rmw_ret;  // error already set
  }

  auto iceoryx_service_names_and_types = rmw_iceoryx_cpp::get_service_names_and_types();

  return rmw_iceoryx_cpp::fill_rmw_names_and_types(
    service_names_and_types, iceoryx_service_names_and_types, allocator);

  return RMW_RET_OK;
}
}  // extern "C"
