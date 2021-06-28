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

#include "rcutils/error_handling.h"

#include "rmw/allocators.h"
#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

extern "C"
{
/// @todo Use the new request/response API of iceoryx v2.0 here instead of dummy services
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

  rmw_service_t * rmw_service = nullptr;

  rmw_service = rmw_service_allocate();
  if (!rmw_service) {
    RMW_SET_ERROR_MSG("failed to allocate memory for service");
    return nullptr;
  }

  void * info = nullptr;

  rmw_service->implementation_identifier = rmw_get_implementation_identifier();
  rmw_service->data = info;

  rmw_service->service_name =
    static_cast<const char *>(rmw_allocate(sizeof(char) * strlen(service_name) + 1));
  if (!rmw_service->service_name) {
    RMW_SET_ERROR_MSG("failed to allocate memory for publisher topic name");
  } else {
    memcpy(const_cast<char *>(rmw_service->service_name), service_name, strlen(service_name) + 1);
  }

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

  rmw_free(const_cast<char *>(service->service_name));
  service->service_name = nullptr;

  rmw_service_free(service);

  return result;
}
}  // extern "C"
