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

#include <time.h>

#include "iceoryx_posh/popo/untyped_subscriber.hpp"
#include "iceoryx_posh/popo/wait_set.hpp"
#include "iceoryx_posh/popo/user_trigger.hpp"

#include "rcutils/error_handling.h"

#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

#include "./types/iceoryx_subscription.hpp"

extern "C"
{
rmw_ret_t
rmw_wait(
  rmw_subscriptions_t * subscriptions,
  rmw_guard_conditions_t * guard_conditions,
  rmw_services_t * services,
  rmw_clients_t * clients,
  rmw_events_t * events,
  rmw_wait_set_t * wait_set,
  const rmw_time_t * wait_timeout)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscriptions, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(guard_conditions, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(services, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(clients, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(events, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(wait_set, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_wait
    : waitset, wait_set->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  iox::popo::WaitSet<iox::MAX_NUMBER_OF_ATTACHMENTS_PER_WAITSET> * waitset =
    static_cast<iox::popo::WaitSet<iox::MAX_NUMBER_OF_ATTACHMENTS_PER_WAITSET> *>(wait_set->data);
  if (!waitset) {
    return RMW_RET_ERROR;
  }

  bool skip_wait{false};
  // attach all iceoryx subscriber to WaitSet
  for (size_t i = 0; i < subscriptions->subscriber_count; ++i) {
    auto iceoryx_subscription =
      static_cast<IceoryxSubscription *>(subscriptions->subscribers[i]);
    auto iceoryx_receiver = iceoryx_subscription->iceoryx_receiver_;

    waitset->attachState(*iceoryx_receiver, iox::popo::SubscriberState::HAS_DATA).or_else(
      [&](auto &) {
        RMW_SET_ERROR_MSG("failed to attach subscriber");
        skip_wait = true;
      });
  }


  // attach all guard conditions to WaitSet
  for (size_t i = 0; i < guard_conditions->guard_condition_count; ++i) {
    auto iceoryx_guard_condition =
      static_cast<iox::popo::UserTrigger *>(guard_conditions->guard_conditions[i]);

    waitset->attachEvent(*iceoryx_guard_condition).or_else(
      [&](auto) {
        RMW_SET_ERROR_MSG("failed to attach guard condition");
        skip_wait = true;
      });
  }

  if (skip_wait) {
    goto after_wait;
  }

  if (!wait_timeout) {
    /// @todo Check triggered subscribers in vector? Is that relevant for rmw?
    auto notificationVector = waitset->wait();
  } else {
    auto sec = iox::units::Duration::fromSeconds(wait_timeout->sec);
    auto nsec = iox::units::Duration::fromNanoseconds(wait_timeout->nsec);
    auto timeout = sec + nsec;

    /// @todo Check triggered subscribers in vector? Is that relevant for rmw?
    auto notificationVector = waitset->timedWait(iox::units::Duration(timeout));
  }

after_wait:
  // reset all the subscriptions that don't have new data
  for (size_t i = 0; i < subscriptions->subscriber_count; ++i) {
    auto iceoryx_subscription =
      static_cast<IceoryxSubscription *>(subscriptions->subscribers[i]);
    iox::popo::UntypedSubscriber * iceoryx_receiver = iceoryx_subscription->iceoryx_receiver_;

    // remove waitset from all receivers because next call a new waitset could be provided
    waitset->detachState(*iceoryx_receiver, iox::popo::SubscriberState::HAS_DATA);

    if (!iceoryx_receiver->hasData()) {
      subscriptions->subscribers[i] = nullptr;
    }
  }

  // reset all the guard_conditions that have not triggered
  for (size_t i = 0; i < guard_conditions->guard_condition_count; ++i) {
    auto iceoryx_guard_condition =
      static_cast<iox::popo::UserTrigger *>(guard_conditions->guard_conditions[i]);

    waitset->detachEvent(*iceoryx_guard_condition);

    if (!iceoryx_guard_condition->hasTriggered()) {
      guard_conditions->guard_conditions[i] = nullptr;
    }
  }

  return RMW_RET_OK;
}
}  // extern "C"
