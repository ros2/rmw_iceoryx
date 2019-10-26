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

#include "./iceoryx_identifier.hpp"
#include "./types/iceoryx_wait_set.hpp"

#include "iceoryx_posh/roudi/introspection_types.hpp"

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

  // Untill we have something better in the iceoryx API,
  // we use the process introspection and its semaphore for notification in the ros waitset
  rmw_wait_set_t * rmw_wait_set = nullptr;
  iox::popo::Subscriber * process_receiver = nullptr;
  iox::posix::Semaphore * semaphore = nullptr;
  IceoryxWaitSet * iceoryx_wait_set = nullptr;

  rmw_wait_set = rmw_wait_set_allocate();
  if (!rmw_wait_set) {
    RMW_SET_ERROR_MSG("failed to allocate wait set");
    goto fail;
  }

  rmw_wait_set->implementation_identifier = rmw_get_implementation_identifier();

  process_receiver = static_cast<iox::popo::Subscriber *>(
    rmw_allocate(sizeof(iox::popo::Subscriber)));
  if (!process_receiver) {
    RMW_SET_ERROR_MSG("failed to allocate memory for wait_set data");
    goto fail;
  }
  RMW_TRY_PLACEMENT_NEW(process_receiver,
    process_receiver,
    goto fail,
    iox::popo::Subscriber,
    iox::roudi::IntrospectionProcessService)

  semaphore = process_receiver->getSemaphore();

  // Set semaphore and subscribe for having wait_set triggers if a new process appears
  process_receiver->setChunkReceiveSemaphore(semaphore);
  process_receiver->subscribe(1);

  iceoryx_wait_set = static_cast<IceoryxWaitSet *>(rmw_allocate(sizeof(IceoryxWaitSet)));
  if (!iceoryx_wait_set) {
    RMW_SET_ERROR_MSG("failed to allocate memory for rmw iceoryx wait_set");
    goto fail;
  }
  RMW_TRY_PLACEMENT_NEW(
    iceoryx_wait_set, iceoryx_wait_set, goto fail, IceoryxWaitSet, semaphore, process_receiver)

  rmw_wait_set->data = static_cast<void *>(iceoryx_wait_set);
  return rmw_wait_set;

fail:
  if (rmw_wait_set) {
    if (process_receiver) {
      RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(process_receiver->~Subscriber(),
        iox::popo::Subscriber)
      rmw_free(process_receiver);
    }

    if (iceoryx_wait_set) {
      RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(
        iceoryx_wait_set->~IceoryxWaitSet(), IceoryxWaitSet)
      rmw_free(iceoryx_wait_set);
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

  auto iceoryx_wait_set = static_cast<IceoryxWaitSet *>(wait_set->data);
  if (iceoryx_wait_set) {
    if (iceoryx_wait_set->iceoryx_receiver_) {
      RMW_TRY_DESTRUCTOR(iceoryx_wait_set->iceoryx_receiver_->~Subscriber(),
        iceoryx_wait_set->iceoryx_receiver_,
        result = RMW_RET_ERROR)
      rmw_free(iceoryx_wait_set->iceoryx_receiver_);
    }
    RMW_TRY_DESTRUCTOR(
      iceoryx_wait_set->~IceoryxWaitSet(), iceoryx_wait_set, result = RMW_RET_ERROR)
    rmw_free(iceoryx_wait_set);
  }
  wait_set->data = nullptr;

  rmw_wait_set_free(wait_set);

  return result;
}
}  // extern "C"
