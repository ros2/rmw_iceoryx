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

#include "./types/iceoryx_subscription.hpp"

#include "iceoryx_posh/popo/subscriber.hpp"

#include "rcutils/error_handling.h"

#include "rmw/event.h"
#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"
#include "rmw/types.h"

#include "rmw_iceoryx_cpp/iceoryx_type_info_introspection.hpp"
#include "rmw_iceoryx_cpp/iceoryx_deserialize.hpp"

#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"

extern "C"
{
rmw_ret_t
rmw_take(
  const rmw_subscription_t * subscription,
  void * ros_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(ros_message, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_ERROR);
  (void)allocation;

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(rmw_take
    : subscription,
    subscription->implementation_identifier,
    rmw_get_implementation_identifier(),
    return RMW_RET_ERROR);

  auto iceoryx_subscription = static_cast<IceoryxSubscription *>(subscription->data);
  if (!iceoryx_subscription) {
    RMW_SET_ERROR_MSG("subscription data is null");
    return RMW_RET_ERROR;
  }

  auto iceoryx_receiver = iceoryx_subscription->iceoryx_receiver_;
  if (!iceoryx_receiver) {
    RMW_SET_ERROR_MSG("iceoryx_receiver is null");
    return RMW_RET_ERROR;
  }

  // Subscription is not matched
  if (iox::popo::SubscriptionState::SUBSCRIBED != iceoryx_receiver->getSubscriptionState()) {
    return RMW_RET_OK;
  }

  // this should never happen if checked already at rmw_create_subscription
  if (!rmw_iceoryx_cpp::iceoryx_is_valid_type_support(&iceoryx_subscription->type_supports_)) {
    RMW_SET_ERROR_MSG("Use either C typesupport or CPP typesupport");
    return RMW_RET_ERROR;
  }

  const iox::mepoo::ChunkInfo * chunk_info = nullptr;
  if (!iceoryx_receiver->getChunkWithInfo(&chunk_info)) {
    RMW_SET_ERROR_MSG("No chunk in iceoryx_receiver");
    return RMW_RET_ERROR;
  }

  // if fixed size, we fetch the data via memcpy
  if (iceoryx_subscription->is_fixed_size_) {
    memcpy(ros_message, chunk_info->m_payload, chunk_info->m_payloadSize);
    iceoryx_receiver->releaseChunkWithInfo(chunk_info);
    *taken = true;
    return RMW_RET_OK;
  }

  // deserialize with cpp typesupport
  auto ts_cpp = get_message_typesupport_handle(
    &iceoryx_subscription->type_supports_,
    rosidl_typesupport_introspection_cpp::typesupport_identifier);
  if (ts_cpp != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(ts_cpp->data);
    rmw_iceoryx_cpp::deserialize(
      static_cast<const char *>(chunk_info->m_payload), members, ros_message);
    iceoryx_receiver->releaseChunkWithInfo(chunk_info);
    *taken = true;
  }

  // deserialize with c typesupport
  auto ts_c = get_message_typesupport_handle(
    &iceoryx_subscription->type_supports_,
    rosidl_typesupport_introspection_c__identifier);
  if (ts_c != nullptr) {
    auto members =
      static_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(ts_c->data);
    rmw_iceoryx_cpp::deserialize(
      static_cast<const char *>(chunk_info->m_payload), members, ros_message);
    iceoryx_receiver->releaseChunkWithInfo(chunk_info);
    *taken = true;
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_take_with_info(
  const rmw_subscription_t * subscription,
  void * ros_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(ros_message, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(message_info, RMW_RET_ERROR);
  (void)allocation;

  // TODO(mphnl) implement message_info related stuff
  (void)message_info;
  return rmw_take(subscription, ros_message, taken, allocation);
}

rmw_ret_t
rmw_take_serialized_message(
  const rmw_subscription_t * subscription,
  rmw_serialized_message_t * serialized_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(serialized_message, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_ERROR);
  (void)allocation;

  assert(false);
  return RMW_RET_OK;
}

rmw_ret_t
rmw_take_serialized_message_with_info(
  const rmw_subscription_t * subscription,
  rmw_serialized_message_t * serialized_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(serialized_message, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(message_info, RMW_RET_ERROR);
  (void)allocation;

  assert(false);
  return RMW_RET_OK;
}

rmw_ret_t
rmw_take_loaned_message(
  const rmw_subscription_t * subscription,
  void ** loaned_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(loaned_message, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_ERROR);
  (void)allocation;

  *taken = false;

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(rmw_take
    : subscription,
    subscription->implementation_identifier,
    rmw_get_implementation_identifier(),
    return RMW_RET_ERROR);

  auto iceoryx_subscription = static_cast<IceoryxSubscription *>(subscription->data);
  if (!iceoryx_subscription) {
    RMW_SET_ERROR_MSG("subscription data is null");
    return RMW_RET_ERROR;
  }

  auto iceoryx_receiver = iceoryx_subscription->iceoryx_receiver_;
  if (!iceoryx_receiver) {
    RMW_SET_ERROR_MSG("iceoryx_receiver is null");
    return RMW_RET_ERROR;
  }

  // this should never happen if checked already at rmw_create_subscription
  if (!rmw_iceoryx_cpp::iceoryx_is_valid_type_support(&iceoryx_subscription->type_supports_)) {
    RMW_SET_ERROR_MSG("Use either C typesupport or CPP typesupport");
    return RMW_RET_ERROR;
  }

  if (!iceoryx_subscription->is_fixed_size_) {
    // TODO(Karsten1987) Alternatively fall back to regular rmw_take with memcpy
    RMW_SET_ERROR_MSG("iceoryx can't take loaned non-fixed size data stuctures");
    return RMW_RET_ERROR;
  }

  if (!iceoryx_receiver->getChunk(const_cast<const void **>(loaned_message))) {
    RMW_SET_ERROR_MSG("No chunk in iceoryx_receiver");
    return RMW_RET_ERROR;
  }
  *taken = true;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_take_loaned_message_with_info(
  const rmw_subscription_t * subscription,
  void ** loaned_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation)
{
  (void) message_info;
  return rmw_take_loaned_message(subscription, loaned_message, taken, allocation);
}

rmw_ret_t
rmw_return_loaned_message_from_subscription(
  const rmw_subscription_t * subscription, void * loaned_message)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(loaned_message, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(rmw_take
    : subscription,
    subscription->implementation_identifier,
    rmw_get_implementation_identifier(),
    return RMW_RET_ERROR);

  auto iceoryx_subscription = static_cast<IceoryxSubscription *>(subscription->data);
  if (!iceoryx_subscription) {
    RMW_SET_ERROR_MSG("subscription data is null");
    return RMW_RET_ERROR;
  }

  auto iceoryx_receiver = iceoryx_subscription->iceoryx_receiver_;
  if (!iceoryx_receiver) {
    RMW_SET_ERROR_MSG("iceoryx_receiver is null");
    return RMW_RET_ERROR;
  }

  auto ret = iceoryx_receiver->releaseChunk(loaned_message);
  return ret ? RMW_RET_OK : RMW_RET_ERROR;
}

rmw_ret_t
rmw_take_event(const rmw_event_t * event_handle, void * event_info, bool * taken)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(event_handle, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(event_info, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_ERROR);

  assert(false);
  return RMW_RET_OK;
}
}  // extern "C"
