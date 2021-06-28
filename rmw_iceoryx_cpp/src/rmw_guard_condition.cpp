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

#include <iostream>

#include "rmw/allocators.h"
#include "rmw/error_handling.h"
#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

#include "./iceoryx_identifier.hpp"
#include "iceoryx_posh/popo/user_trigger.hpp"

extern "C"
{
rmw_guard_condition_t *
rmw_create_guard_condition(rmw_context_t * context)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(context, nullptr);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_create_guard_condition
    : context,
    context->implementation_identifier,
    rmw_get_implementation_identifier(),
    /// @todo wwjwood: replace this with RMW_RET_INCORRECT_RMW_IMPLEMENTATION when
    /// possible
    return nullptr);

  rmw_guard_condition_t * guard_condition = nullptr;
  iox::popo::UserTrigger * iceoryx_guard_condition = nullptr;

  guard_condition = rmw_guard_condition_allocate();
  if (!guard_condition) {
    RMW_SET_ERROR_MSG("failed to construct guard condition");
    goto fail;
  }
  guard_condition->implementation_identifier = rmw_get_implementation_identifier();

  iceoryx_guard_condition = static_cast<iox::popo::UserTrigger *>(
    rmw_allocate(sizeof(iox::popo::UserTrigger)));
  if (!iceoryx_guard_condition) {
    RMW_SET_ERROR_MSG("failed to construct guard condition data");
    goto fail;
  }
  RMW_TRY_PLACEMENT_NEW(
    iceoryx_guard_condition,
    iceoryx_guard_condition,
    goto fail,
    // cppcheck-suppress syntaxError
    iox::popo::UserTrigger, )
  guard_condition->data = iceoryx_guard_condition;

  return guard_condition;

fail:
  if (guard_condition) {
    if (iceoryx_guard_condition) {
      RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(
        iceoryx_guard_condition->~UserTrigger(), iceoryx_guard_condition)
      rmw_free(iceoryx_guard_condition);
    }
    rmw_guard_condition_free(guard_condition);
  }

  return nullptr;
}

rmw_ret_t
rmw_destroy_guard_condition(rmw_guard_condition_t * guard_condition)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(guard_condition, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_destroy_guard_condition
    : guard_condition,
    guard_condition->implementation_identifier,
    rmw_get_implementation_identifier(),
    return RMW_RET_ERROR);

  auto iceoryx_guard_condition = static_cast<iox::popo::UserTrigger *>(guard_condition->data);

  auto result = RMW_RET_OK;
  if (iceoryx_guard_condition) {
    RMW_TRY_DESTRUCTOR(
      iceoryx_guard_condition->~UserTrigger(),
      iceoryx_guard_condition,
      result = RMW_RET_ERROR)
    rmw_free(iceoryx_guard_condition);
  }
  rmw_guard_condition_free(guard_condition);

  return result;
}
}  // extern "C"
