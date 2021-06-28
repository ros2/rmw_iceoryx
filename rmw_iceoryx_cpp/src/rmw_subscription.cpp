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

#include <string>

#include "iceoryx_posh/capro/service_description.hpp"

#include "rcutils/error_handling.h"

#include "rmw/allocators.h"
#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

#include "rmw_iceoryx_cpp/iceoryx_name_conversion.hpp"

#include "./types/iceoryx_subscription.hpp"

extern "C"
{
rmw_ret_t
rmw_init_subscription_allocation(
  const rosidl_message_type_support_t * type_supports,
  const rosidl_runtime_c__Sequence__bound * message_bounds,
  rmw_subscription_allocation_t * allocation)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(type_supports, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(message_bounds, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(allocation, RMW_RET_ERROR);  // might be null

  RMW_SET_ERROR_MSG("rmw_iceoryx_cpp does not support subscription allocations.");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_fini_subscription_allocation(rmw_subscription_allocation_t * allocation)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(allocation, RMW_RET_ERROR);

  RMW_SET_ERROR_MSG("rmw_iceoryx_cpp does not support subscription allocations.");
  return RMW_RET_UNSUPPORTED;
}

rmw_subscription_t *
rmw_create_subscription(
  const rmw_node_t * node,
  const rosidl_message_type_support_t * type_supports,
  const char * topic_name,
  const rmw_qos_profile_t * qos_policies,
  const rmw_subscription_options_t * subscription_options)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(type_supports, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(topic_name, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(qos_policies, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription_options, nullptr);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_create_subscription
    : node, node->implementation_identifier, rmw_get_implementation_identifier(), return nullptr);

  // create the iceoryx service description for a receiver
  auto service_description =
    rmw_iceoryx_cpp::get_iceoryx_service_description(topic_name, type_supports);

  std::string node_full_name = std::string(node->namespace_) + std::string(node->name);
  rmw_subscription_t * rmw_subscription = nullptr;
  iox::popo::UntypedSubscriber * iceoryx_receiver = nullptr;
  IceoryxSubscription * iceoryx_subscription = nullptr;

  rmw_subscription = rmw_subscription_allocate();
  if (!rmw_subscription) {
    RMW_SET_ERROR_MSG("failed to allocate subscription");
    goto fail;
  }

  iceoryx_receiver =
    static_cast<iox::popo::UntypedSubscriber *>(rmw_allocate(
      sizeof(iox::popo::UntypedSubscriber)));
  if (!iceoryx_receiver) {
    RMW_SET_ERROR_MSG("failed to allocate memory for iceoryx receiver");
    goto fail;
  }
  RMW_TRY_PLACEMENT_NEW(
    iceoryx_receiver, iceoryx_receiver, goto fail,
    iox::popo::UntypedSubscriber, service_description,
    iox::popo::SubscriberOptions{
      qos_policies->depth, 0U, iox::NodeName_t(iox::cxx::TruncateToCapacity, node_full_name)});

  // instant subscribe, queue size form qos settings
  iceoryx_receiver->subscribe();

  iceoryx_subscription =
    static_cast<IceoryxSubscription *>(rmw_allocate(sizeof(IceoryxSubscription)));
  if (!iceoryx_subscription) {
    RMW_SET_ERROR_MSG("failed to allocate memory for rmw iceoryx subscription");
    goto fail;
  }
  RMW_TRY_PLACEMENT_NEW(
    iceoryx_subscription, iceoryx_subscription,
    goto fail, IceoryxSubscription, type_supports, iceoryx_receiver)

  rmw_subscription->implementation_identifier = rmw_get_implementation_identifier();
  rmw_subscription->data = iceoryx_subscription;
  rmw_subscription->topic_name =
    static_cast<const char *>(rmw_allocate(sizeof(char) * strlen(topic_name) + 1));
  if (!rmw_subscription->topic_name) {
    RMW_SET_ERROR_MSG("failed to allocate memory for subscription topic name");
    goto fail;
  }
  memcpy(const_cast<char *>(rmw_subscription->topic_name), topic_name, strlen(topic_name) + 1);

  rmw_subscription->can_loan_messages = iceoryx_subscription->is_fixed_size_;

  return rmw_subscription;

fail:
  if (rmw_subscription) {
    if (iceoryx_receiver) {
      /// @todo Can we avoid to use the impl here?
      RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(
        iceoryx_receiver->~UntypedSubscriberImpl(), iox::popo::UntypedSubscriber)
      rmw_free(iceoryx_receiver);
    }
    if (iceoryx_subscription) {
      RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(
        iceoryx_subscription->~IceoryxSubscription(), IceoryxSubscription)
      rmw_free(iceoryx_subscription);
    }
    if (rmw_subscription->topic_name) {
      rmw_free(const_cast<char *>(rmw_subscription->topic_name));
    }
    rmw_subscription_free(rmw_subscription);
  }

  return nullptr;
}

rmw_ret_t
rmw_subscription_get_actual_qos(
  const rmw_subscription_t * subscription,
  rmw_qos_profile_t * qos)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_ERROR);

  /// @todo poehnl: check in detail
  *qos = rmw_qos_profile_default;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_destroy_subscription(
  rmw_node_t * node,
  rmw_subscription_t * subscription)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_destroy_subscription
    : subscription, subscription->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  rmw_ret_t result = RMW_RET_OK;

  IceoryxSubscription * iceoryx_subscription =
    static_cast<IceoryxSubscription *>(subscription->data);
  if (iceoryx_subscription) {
    if (iceoryx_subscription->iceoryx_receiver_) {
      // @todo Can we avoid to use the impl here?
      RMW_TRY_DESTRUCTOR(
        iceoryx_subscription->iceoryx_receiver_->~UntypedSubscriberImpl(),
        iceoryx_subscription->iceoryx_receiver_,
        result = RMW_RET_ERROR)
      rmw_free(iceoryx_subscription->iceoryx_receiver_);
    }
    RMW_TRY_DESTRUCTOR(
      iceoryx_subscription->~IceoryxSubscription(),
      iceoryx_subscription,
      result = RMW_RET_ERROR)
    rmw_free(iceoryx_subscription);
  }
  subscription->data = nullptr;

  rmw_free(const_cast<char *>(subscription->topic_name));
  subscription->topic_name = nullptr;

  rmw_subscription_free(subscription);

  return result;
}
}  // extern "C"
