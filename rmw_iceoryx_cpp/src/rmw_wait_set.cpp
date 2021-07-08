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

#include "./iceoryx_identifier.hpp"

#include "iceoryx_posh/roudi/introspection_types.hpp"
#include "iceoryx_posh/popo/wait_set.hpp"

#include "rcutils/error_handling.h"

#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

extern "C"
{
rmw_wait_set_t *
rmw_create_wait_set(rmw_context_t * context, size_t max_conditions)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(context, nullptr);
  (void)max_conditions;

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_create_wait_set
    : context, context->implementation_identifier,
    rmw_get_implementation_identifier(), return nullptr);

  rmw_wait_set_t * rmw_wait_set = nullptr;
  iox::popo::WaitSet<iox::MAX_NUMBER_OF_ATTACHMENTS_PER_WAITSET> * waitset = nullptr;

  rmw_wait_set = rmw_wait_set_allocate();
  if (!rmw_wait_set) {
    RMW_SET_ERROR_MSG("failed to allocate wait set");
    goto fail;
  }

  rmw_wait_set->implementation_identifier = rmw_get_implementation_identifier();

  // create waitset
  waitset = static_cast<iox::popo::WaitSet<iox::MAX_NUMBER_OF_ATTACHMENTS_PER_WAITSET> *>(
    rmw_allocate(sizeof(iox::popo::WaitSet<iox::MAX_NUMBER_OF_ATTACHMENTS_PER_WAITSET>)));
  if (!waitset) {
    RMW_SET_ERROR_MSG("failed to allocate memory for wait_set data");
    goto fail;
  }
  RMW_TRY_PLACEMENT_NEW(
    waitset,
    waitset,
    goto fail,
    iox::popo::WaitSet<iox::MAX_NUMBER_OF_ATTACHMENTS_PER_WAITSET>);

  rmw_wait_set->data = static_cast<void *>(waitset);
  return rmw_wait_set;

fail:
  if (rmw_wait_set) {
    if (waitset) {
      RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(
        waitset->~WaitSet(),
        iox::popo::WaitSet<iox::MAX_NUMBER_OF_ATTACHMENTS_PER_WAITSET>)
      rmw_free(waitset);
    }

    rmw_wait_set_free(rmw_wait_set);
  }
  return nullptr;
}

rmw_ret_t
rmw_destroy_wait_set(rmw_wait_set_t * wait_set)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(wait_set, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_destroy_wait_set
    : waitset, wait_set->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  rmw_ret_t result = RMW_RET_OK;

  auto iceoryx_wait_set =
    static_cast<iox::popo::WaitSet<iox::MAX_NUMBER_OF_ATTACHMENTS_PER_WAITSET> *>(wait_set->data);

  if (iceoryx_wait_set) {
    RMW_TRY_DESTRUCTOR(
      iceoryx_wait_set->~WaitSet(),
      iceoryx_wait_set,
      result = RMW_RET_ERROR)
    rmw_free(iceoryx_wait_set);
  }
  wait_set->data = nullptr;

  rmw_wait_set_free(wait_set);

  return result;
}
}  // extern "C"
