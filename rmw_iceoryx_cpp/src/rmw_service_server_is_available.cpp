// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2023 by Apex.AI Inc. All rights reserved.
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

#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

#include "./types/iceoryx_client.hpp"

//#include "iceoryx_posh/runtime/service_discovery.hpp"

extern "C"
{
rmw_ret_t
rmw_service_server_is_available(
  const rmw_node_t * node,
  const rmw_client_t * client,
  bool * is_available)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(is_available, RMW_RET_ERROR);

  rmw_ret_t ret = RMW_RET_ERROR;

  auto iceoryx_client_abstraction = static_cast<IceoryxClient *>(client->data);
  if (!iceoryx_client_abstraction) {
    RMW_SET_ERROR_MSG("client data is null");
    ret = RMW_RET_ERROR;
    return ret;
  }

  auto iceoryx_client = iceoryx_client_abstraction->iceoryx_client_;
  if (!iceoryx_client) {
    RMW_SET_ERROR_MSG("iceoryx_client is null");
    ret = RMW_RET_ERROR;
    return ret;
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));

  if (iceoryx_client->getConnectionState() == iox::ConnectionState::CONNECTED) {
    *is_available = true;
  }

  /// @todo Better to go through service discovery?
  // iox::runtime::ServiceDiscovery serviceDiscovery;
  // auto& searchItem = iceoryx_client->getServiceDescription();
  // serviceDiscovery.findService( searchItem.getServiceIDString(),
  //                               searchItem.getInstanceIDString(),
  //                               searchItem.getEventIDString(),
  //                               [&](auto&){ *is_available = true; },
  //                               iox::popo::MessagingPattern::REQ_RES);

  ret = RMW_RET_OK;
  return ret;
}
}  // extern "C"
