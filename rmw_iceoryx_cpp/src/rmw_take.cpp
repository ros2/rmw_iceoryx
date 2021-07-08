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

#include "./types/iceoryx_subscription.hpp"

#include "iceoryx_posh/popo/untyped_subscriber.hpp"

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

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_take
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
  if (iox::SubscribeState::SUBSCRIBED != iceoryx_receiver->getSubscriptionState()) {
    return RMW_RET_OK;
  }

  // this should never happen if checked already at rmw_create_subscription
  if (!rmw_iceoryx_cpp::iceoryx_is_valid_type_support(&iceoryx_subscription->type_supports_)) {
    RMW_SET_ERROR_MSG("Use either C typesupport or CPP typesupport");
    return RMW_RET_ERROR;
  }

  const iox::mepoo::ChunkHeader * chunk_header = nullptr;
  const void * user_payload = nullptr;

  rmw_ret_t ret = RMW_RET_ERROR;
  iceoryx_receiver->take()
  .and_then(
    [&](const void * userPayload) {
      user_payload = userPayload;
      chunk_header = iox::mepoo::ChunkHeader::fromUserPayload(user_payload);
      ret = RMW_RET_OK;
    })
  .or_else(
    [&](iox::popo::ChunkReceiveResult) {
      RMW_SET_ERROR_MSG("No chunk in iceoryx_receiver");
      ret = RMW_RET_ERROR;
    });

  /// @todo move this to lambda in a function?
  if (ret == RMW_RET_ERROR) {
    return ret;
  }

  // if fixed size, we fetch the data via memcpy
  if (iceoryx_subscription->is_fixed_size_) {
    memcpy(ros_message, user_payload, chunk_header->userPayloadSize());
    iceoryx_receiver->release(user_payload);
    *taken = true;
    ret = RMW_RET_OK;
  } else {
    rmw_iceoryx_cpp::deserialize(
      static_cast<const char *>(user_payload),
      &iceoryx_subscription->type_supports_,
      ros_message);
    iceoryx_receiver->release(user_payload);
    *taken = true;
    ret = RMW_RET_OK;
  }

  return ret;
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

  /// @todo poehnl: implement message_info related stuff
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

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_take
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
  if (iox::SubscribeState::SUBSCRIBED != iceoryx_receiver->getSubscriptionState()) {
    return RMW_RET_OK;
  }

  // this should never happen if checked already at rmw_create_subscription
  if (!rmw_iceoryx_cpp::iceoryx_is_valid_type_support(&iceoryx_subscription->type_supports_)) {
    RMW_SET_ERROR_MSG("Use either C typesupport or CPP typesupport");
    return RMW_RET_ERROR;
  }

  const iox::mepoo::ChunkHeader * chunk_header = nullptr;
  const void * user_payload = nullptr;

  rmw_ret_t ret = RMW_RET_OK;
  iceoryx_receiver->take()
  .and_then(
    [&](const void * userPayload) {
      user_payload = userPayload;
      chunk_header = iox::mepoo::ChunkHeader::fromUserPayload(user_payload);
    })
  .or_else(
    [&](iox::popo::ChunkReceiveResult) {
      RMW_SET_ERROR_MSG("No chunk in iceoryx_receiver");
      ret = RMW_RET_ERROR;
    });

  // all incoming data is serialzed already in memory, so simply call memcopy
  ret = rmw_serialized_message_resize(serialized_message, chunk_header->userPayloadSize());
  if (RMW_RET_OK == ret) {
    memcpy(serialized_message->buffer, user_payload, chunk_header->userPayloadSize());
    serialized_message->buffer_length = chunk_header->userPayloadSize();
    iceoryx_receiver->release(user_payload);
    *taken = true;
  }

  return ret;
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

  /// @todo poehnl: implement message_info related stuff
  (void) message_info;
  return rmw_take_serialized_message(subscription, serialized_message, taken, allocation);
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

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_take
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
    /// @todo Karsten1987: Alternatively fall back to regular rmw_take with memcpy
    RMW_SET_ERROR_MSG("iceoryx can't take loaned non-fixed size data stuctures");
    return RMW_RET_ERROR;
  }

  rmw_ret_t ret = RMW_RET_OK;
  iceoryx_receiver->take()
  .and_then(
    [&](const void * userPayload) {
      *loaned_message = const_cast<void *>(userPayload);
      ret = RMW_RET_OK;
    })
  .or_else(
    [&](auto &) {
      RMW_SET_ERROR_MSG("No chunk in iceoryx_receiver");
      ret = RMW_RET_ERROR;
    });
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

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_take
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

  iceoryx_receiver->release(loaned_message);
  return RMW_RET_OK;
}

rmw_ret_t
rmw_take_event(const rmw_event_t * event_handle, void * event_info, bool * taken)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(event_handle, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(event_info, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_ERROR);

  RMW_SET_ERROR_MSG("rmw_iceoryx_cpp does not support events.");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_take_sequence(
  const rmw_subscription_t * subscription,
  size_t count,
  rmw_message_sequence_t * message_sequence,
  rmw_message_info_sequence_t * message_info_sequence,
  size_t * taken,
  rmw_subscription_allocation_t * allocation)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_ERROR);
  (void) count;
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(message_sequence, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(message_info_sequence, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(allocation, RMW_RET_ERROR);

  RMW_SET_ERROR_MSG("rmw_iceoryx_cpp does not support rmw_take_sequence.");
  return RMW_RET_UNSUPPORTED;
}
}  // extern "C"
