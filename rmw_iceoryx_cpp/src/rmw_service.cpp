// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2022 by Apex.AI Inc. All rights reserved.
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

#include "rmw/allocators.h"
#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

#include "rmw_iceoryx_cpp/iceoryx_name_conversion.hpp"

#include "types/iceoryx_server.hpp"

extern "C"
{
rmw_service_t *
rmw_create_service(
  const rmw_node_t * node,
  const rosidl_service_type_support_t * type_supports,
  const char * service_name,
  const rmw_qos_profile_t * qos_policies)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(type_supports, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(service_name, nullptr);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(qos_policies, nullptr);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
      rmw_create_service
      : node, node->implementation_identifier, rmw_get_implementation_identifier(), return nullptr);

  // create the iceoryx service description for a sender
  auto service_description =
    rmw_iceoryx_cpp::get_iceoryx_service_description(service_name, type_supports);

  std::string node_full_name = std::string(node->namespace_) + std::string(node->name);
  rmw_service_t * rmw_service = nullptr;
  iox::popo::UntypedServer * iceoryx_server = nullptr;

  rmw_service = rmw_service_allocate();
  if (!rmw_service) {
    RMW_SET_ERROR_MSG("failed to allocate memory for service");
    return nullptr;
  }

  auto cleanupAfterError = [](){};

  iceoryx_server =
    static_cast<iox::popo::UntypedServer *>(rmw_allocate(
      sizeof(iox::popo::UntypedServer)));
  if (!iceoryx_server) {
    RMW_SET_ERROR_MSG("failed to allocate memory for iceoryx server");
    cleanupAfterError();
    return nullptr;
  }

  RMW_TRY_PLACEMENT_NEW(
    iceoryx_server, iceoryx_server,
    cleanupAfterError(), iox::popo::UntypedServer, service_description,
    iox::popo::ServerOptions{
      0U, iox::NodeName_t(iox::cxx::TruncateToCapacity, node_full_name)});

  rmw_service->implementation_identifier = rmw_get_implementation_identifier();
  rmw_service->data = iceoryx_server;

  rmw_service->service_name =
    static_cast<const char *>(rmw_allocate(sizeof(char) * strlen(service_name) + 1));
  if (!rmw_service->service_name) {
    RMW_SET_ERROR_MSG("failed to allocate memory for service name");
    cleanupAfterError();
    return nullptr;
  } else {
    memcpy(const_cast<char *>(rmw_service->service_name), service_name, strlen(service_name) + 1);
  }

  /// @todo allocate IceoryxServer here and fill size_t message_size_ and message_alignment_;

  return rmw_service;
}

rmw_ret_t
rmw_destroy_service(rmw_node_t * node, rmw_service_t * service)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(service, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_destroy_service
    : service, service->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  rmw_ret_t result = RMW_RET_OK;

  iox::popo::UntypedServer * iceoryx_server = static_cast<iox::popo::UntypedServer *>(service->data);
  if (iceoryx_server) {
      RMW_TRY_DESTRUCTOR(
        iceoryx_server->~UntypedServerImpl(),
        iceoryx_server,
        result = RMW_RET_ERROR)
      rmw_free(iceoryx_server);
  }

  service->data = nullptr;

  rmw_free(const_cast<char *>(service->service_name));
  service->service_name = nullptr;

  rmw_service_free(service);

  return result;
}

rmw_ret_t rmw_service_response_publisher_get_actual_qos(
  const rmw_service_t * service,
  rmw_qos_profile_t * qos)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(service, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  *qos = rmw_qos_profile_default;

  return RMW_RET_OK;
}

rmw_ret_t rmw_service_request_subscription_get_actual_qos(
  const rmw_service_t * service,
  rmw_qos_profile_t * qos)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(service, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  *qos = rmw_qos_profile_default;

  return RMW_RET_OK;
}

rmw_ret_t rmw_service_set_on_new_request_callback(
  rmw_service_t * service,
  rmw_event_callback_t callback,
  const void * user_data)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(service, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(callback, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(user_data, RMW_RET_INVALID_ARGUMENT);

  return RMW_RET_UNSUPPORTED;
}
}  // extern "C"
