// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2022 - 2023 by Apex.AI Inc. All rights reserved.
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

#include <assert.h>

#include "rcutils/error_handling.h"

#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

#include "rmw_iceoryx_cpp/iceoryx_name_conversion.hpp"

#include "types/iceoryx_client.hpp"

extern "C"
{
rmw_client_t *
rmw_create_client(
  const rmw_node_t * node,
  const rosidl_service_type_support_t * type_supports,
  const char * service_name,
  const rmw_qos_profile_t * qos_policies)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, NULL);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(type_supports, NULL);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(service_name, NULL);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(qos_policies, NULL);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_create_client
    : node, node->implementation_identifier, rmw_get_implementation_identifier(), return nullptr);

  // create the iceoryx service description for a sender
  auto service_description =
    rmw_iceoryx_cpp::get_iceoryx_service_description(service_name, type_supports);

  std::string node_full_name = std::string(node->namespace_) + std::string(node->name);
  rmw_client_t * rmw_client = nullptr;
  iox::popo::UntypedClient * iceoryx_client = nullptr;
  IceoryxClient * iceoryx_client_abstraction = nullptr;

  bool returnOnError = false;

  auto cleanupAfterError = [&]() {
      if (rmw_client) {
        if (iceoryx_client) {
          RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(
            iceoryx_client->~UntypedClient(), iox::popo::UntypedClient)
          rmw_free(iceoryx_client);
        }
        if (iceoryx_client_abstraction) {
          RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(
            iceoryx_client_abstraction->~IceoryxClient(), IceoryxClient)
          rmw_free(iceoryx_client_abstraction);
        }
        if (rmw_client->service_name) {
          rmw_free(const_cast<char *>(rmw_client->service_name));
        }
        rmw_client_free(rmw_client);
        returnOnError = true;
      }
    };

  rmw_client = rmw_client_allocate();
  if (!rmw_client) {
    RMW_SET_ERROR_MSG("failed to allocate memory for client");
    cleanupAfterError();
    return nullptr;
  }

  iceoryx_client =
    static_cast<iox::popo::UntypedClient *>(rmw_allocate(
      sizeof(iox::popo::UntypedClient)));
  if (!iceoryx_client) {
    RMW_SET_ERROR_MSG("failed to allocate memory for iceoryx client");
    cleanupAfterError();
    return nullptr;
  }

  RMW_TRY_PLACEMENT_NEW(
    iceoryx_client, iceoryx_client,
    cleanupAfterError(), iox::popo::UntypedClient, service_description,
    iox::popo::ClientOptions{
      qos_policies->depth, iox::into<iox::lossy<iox::NodeName_t>>(node_full_name)});
  if (returnOnError) {
    return nullptr;
  }

  iceoryx_client->connect();

  iceoryx_client_abstraction =
    static_cast<IceoryxClient *>(rmw_allocate(sizeof(IceoryxClient)));
  if (!iceoryx_client_abstraction) {
    RMW_SET_ERROR_MSG("failed to allocate memory for rmw iceoryx publisher");
    cleanupAfterError();
    return nullptr;
  }
  RMW_TRY_PLACEMENT_NEW(
    iceoryx_client_abstraction, iceoryx_client_abstraction,
    cleanupAfterError(), IceoryxClient, type_supports, iceoryx_client);
  if (returnOnError) {
    return nullptr;
  }

  rmw_client->implementation_identifier = rmw_get_implementation_identifier();
  rmw_client->data = iceoryx_client_abstraction;

  rmw_client->service_name =
    static_cast<const char *>(rmw_allocate(sizeof(char) * strlen(service_name) + 1));
  if (!rmw_client->service_name) {
    RMW_SET_ERROR_MSG("failed to allocate memory for service name");
    cleanupAfterError();
    return nullptr;
  }
  memcpy(const_cast<char *>(rmw_client->service_name), service_name, strlen(service_name) + 1);
  return rmw_client;
}

rmw_ret_t
rmw_destroy_client(
  rmw_node_t * node,
  rmw_client_t * client)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_destroy_client
    : client, client->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  rmw_ret_t result = RMW_RET_OK;

  IceoryxClient * iceoryx_client_abstraction = static_cast<IceoryxClient *>(client->data);
  if (iceoryx_client_abstraction) {
    if (iceoryx_client_abstraction->iceoryx_client_) {
      RMW_TRY_DESTRUCTOR(
        iceoryx_client_abstraction->iceoryx_client_->~UntypedClient(),
        iceoryx_client_abstraction->iceoryx_client_,
        result = RMW_RET_ERROR)
      rmw_free(iceoryx_client_abstraction->iceoryx_client_);
    }
    RMW_TRY_DESTRUCTOR(
      iceoryx_client_abstraction->~IceoryxClient(),
      iceoryx_client_abstraction,
      result = RMW_RET_ERROR)
    rmw_free(iceoryx_client_abstraction);
  }

  client->data = nullptr;

  rmw_free(const_cast<char *>(client->service_name));
  client->service_name = nullptr;

  rmw_client_free(client);
  return result;
}

rmw_ret_t rmw_client_request_publisher_get_actual_qos(
  const rmw_client_t * client,
  rmw_qos_profile_t * qos)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  *qos = rmw_qos_profile_default;

  return RMW_RET_OK;
}

rmw_ret_t rmw_client_response_subscription_get_actual_qos(
  const rmw_client_t * client,
  rmw_qos_profile_t * qos)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  *qos = rmw_qos_profile_default;

  return RMW_RET_OK;
}

rmw_ret_t rmw_client_set_on_new_response_callback(
  rmw_client_t * client,
  rmw_event_callback_t callback,
  const void * user_data)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(callback, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(user_data, RMW_RET_INVALID_ARGUMENT);

  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_get_gid_for_client(const rmw_client_t * client, rmw_gid_t * gid)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(gid, RMW_RET_INVALID_ARGUMENT);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_get_gid_for_client
    : client, client->implementation_identifier,
    rmw_get_implementation_identifier(),
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  IceoryxClient * iceoryx_client_abstraction = static_cast<IceoryxClient *>(client->data);

  if (!iceoryx_client_abstraction) {
    RMW_SET_ERROR_MSG("client info handle is null");
    return RMW_RET_INVALID_ARGUMENT;
  }
  *gid = iceoryx_client_abstraction->gid_;
  return RMW_RET_OK;
}
}  // extern "C"
