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

#include "rmw_iceoryx_cpp/iceoryx_name_conversion.hpp"

#include "./types/iceoryx_publisher.hpp"

extern "C"
{
rmw_ret_t
rmw_init_publisher_allocation(
  const rosidl_message_type_support_t * type_support,
  const rosidl_runtime_c__Sequence__bound * message_bounds,
  rmw_publisher_allocation_t * allocation)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(type_support, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(message_bounds, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(allocation, RMW_RET_ERROR);

  RMW_SET_ERROR_MSG("rmw_iceoryx_cpp does not support publisher allocations.");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_fini_publisher_allocation(rmw_publisher_allocation_t * allocation)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(allocation, RMW_RET_ERROR);

  RMW_SET_ERROR_MSG("rmw_iceoryx_cpp does not support publisher allocations.");
  return RMW_RET_UNSUPPORTED;
}

rmw_publisher_t *
rmw_create_publisher(
  const rmw_node_t * node,
  const rosidl_message_type_support_t * type_supports,
  const char * topic_name,
  const rmw_qos_profile_t * qos_policies,
  const rmw_publisher_options_t * publisher_options)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(type_supports, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(topic_name, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(qos_policies, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(publisher_options, nullptr);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_create_publisher
    : node, node->implementation_identifier, rmw_get_implementation_identifier(), return nullptr);

  // create the iceoryx service description for a sender
  auto service_description =
    rmw_iceoryx_cpp::get_iceoryx_service_description(topic_name, type_supports);

  std::string node_full_name = std::string(node->namespace_) + std::string(node->name);
  rmw_publisher_t * rmw_publisher = nullptr;
  iox::popo::UntypedPublisher * iceoryx_sender = nullptr;
  IceoryxPublisher * iceoryx_publisher = nullptr;

  // allocate rmw_publisher
  rmw_publisher = rmw_publisher_allocate();
  if (!rmw_publisher) {
    RMW_SET_ERROR_MSG("failed to allocate rmw_publisher_t");
    goto fail;
  }

  // allocate iceoryx_sender
  iceoryx_sender =
    static_cast<iox::popo::UntypedPublisher *>(rmw_allocate(
      sizeof(iox::popo::UntypedPublisher)));
  if (!iceoryx_sender) {
    RMW_SET_ERROR_MSG("failed to allocate memory for iceoryx sender");
    goto fail;
  }

  RMW_TRY_PLACEMENT_NEW(
    iceoryx_sender, iceoryx_sender,
    goto fail, iox::popo::UntypedPublisher, service_description,
    iox::popo::PublisherOptions{
      0U, iox::NodeName_t(iox::cxx::TruncateToCapacity, node_full_name)});

  iceoryx_sender->offer();  // make the sender visible

  // allocate iceoryx_publisher
  iceoryx_publisher =
    static_cast<IceoryxPublisher *>(rmw_allocate(sizeof(IceoryxPublisher)));
  if (!iceoryx_publisher) {
    RMW_SET_ERROR_MSG("failed to allocate memory for rmw iceoryx publisher");
    goto fail;
  }
  RMW_TRY_PLACEMENT_NEW(
    iceoryx_publisher, iceoryx_publisher,
    goto fail, IceoryxPublisher, type_supports, iceoryx_sender);

  // compose rmw_publisher
  rmw_publisher->implementation_identifier = rmw_get_implementation_identifier();
  rmw_publisher->data = iceoryx_publisher;
  rmw_publisher->topic_name =
    static_cast<const char *>(rmw_allocate(sizeof(char) * strlen(topic_name) + 1));
  if (!rmw_publisher->topic_name) {
    RMW_SET_ERROR_MSG("failed to allocate memory for publisher topic name");
    goto fail;
  }
  memcpy(const_cast<char *>(rmw_publisher->topic_name), topic_name, strlen(topic_name) + 1);
  rmw_publisher->can_loan_messages = iceoryx_publisher->is_fixed_size_;

  return rmw_publisher;

fail:
  if (rmw_publisher) {
    if (iceoryx_sender) {
      RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(
        iceoryx_sender->~UntypedPublisherImpl(), iox::popo::UntypedPublisher)
      rmw_free(iceoryx_sender);
    }
    if (iceoryx_publisher) {
      RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(
        iceoryx_publisher->~IceoryxPublisher(), IceoryxPublisher)
      rmw_free(iceoryx_publisher);
    }
    if (rmw_publisher->topic_name) {
      rmw_free(const_cast<char *>(rmw_publisher->topic_name));
    }
    rmw_publisher_free(rmw_publisher);
  }

  return nullptr;
}

rmw_ret_t
rmw_publisher_assert_liveliness(const rmw_publisher_t * publisher)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_ERROR);

  RMW_SET_ERROR_MSG("rmw_iceoryx_cpp does not support liveliness.");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_publisher_wait_for_all_acked(const rmw_publisher_t * publisher, rmw_time_t wait_timeout)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_ERROR);
  (void)wait_timeout;

  RMW_SET_ERROR_MSG("rmw_iceoryx_cpp does not support wait for all acked.");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_publisher_get_actual_qos(const rmw_publisher_t * publisher, rmw_qos_profile_t * qos)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_ERROR);

  (void)publisher;

  /// @todo poehnl: check in detail
  *qos = rmw_qos_profile_default;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_destroy_publisher(rmw_node_t * node, rmw_publisher_t * publisher)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_destroy_publisher
    : publisher, publisher->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  rmw_ret_t result = RMW_RET_OK;

  IceoryxPublisher * iceoryx_publisher = static_cast<IceoryxPublisher *>(publisher->data);
  if (iceoryx_publisher) {
    if (iceoryx_publisher->iceoryx_sender_) {
      RMW_TRY_DESTRUCTOR(
        iceoryx_publisher->iceoryx_sender_->~UntypedPublisherImpl(),
        iceoryx_publisher->iceoryx_sender_,
        result = RMW_RET_ERROR)
      rmw_free(iceoryx_publisher->iceoryx_sender_);
    }
    RMW_TRY_DESTRUCTOR(
      iceoryx_publisher->~IceoryxPublisher(),
      iceoryx_publisher,
      result = RMW_RET_ERROR)
    rmw_free(iceoryx_publisher);
  }
  publisher->data = nullptr;

  rmw_free(const_cast<char *>(publisher->topic_name));
  publisher->topic_name = nullptr;

  rmw_publisher_free(publisher);

  return result;
}
}  // extern "C"
