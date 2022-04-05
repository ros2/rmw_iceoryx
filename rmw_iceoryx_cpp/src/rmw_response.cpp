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

#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

#include "iceoryx_posh/popo/untyped_client.hpp"

extern "C"
{
rmw_ret_t
rmw_take_response(
  const rmw_client_t * client,
  rmw_service_info_t * request_header,
  void * ros_response,
  bool * taken)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(request_header, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(ros_response, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_take_response
    : client,
    client->implementation_identifier,
    rmw_get_implementation_identifier(),
    return RMW_RET_ERROR);

  auto iceoryx_client = static_cast<iox::popo::UntypedClient *>(client->data);
  if (!iceoryx_client) {
    RMW_SET_ERROR_MSG("client data is null");
    return RMW_RET_ERROR;
  }

  const iox::mepoo::ChunkHeader * chunk_header = nullptr;
  const void * user_payload = nullptr;
  rmw_ret_t ret = RMW_RET_ERROR;

  iceoryx_client->take()
  .and_then(
    [&](const void * responsePayload) {
      auto responseHeader = iox::popo::ResponseHeader::fromPayload(responsePayload);
      /// @todo check writer guid
      if (responseHeader->getSequenceId() == request_header->request_id.sequence_number)
      {
        user_payload = responseHeader;
        chunk_header = iox::mepoo::ChunkHeader::fromUserPayload(user_payload);
        ret = RMW_RET_OK;
      }
      else
      {
          std::cout << "Got Response with outdated sequence number!" << std::endl;
        ret = RMW_RET_ERROR;

      }
    })
  .or_else(
    [&](iox::popo::ChunkReceiveResult) {
      RMW_SET_ERROR_MSG("No chunk in iceoryx_client");
      ret = RMW_RET_ERROR;
    });

  if (ret == RMW_RET_ERROR) {
    return ret;
  }

  /// @todo a wrapper class of client is needed to save the fixed size info
  // if fixed size, we fetch the data via memcpy
  //if (iceoryx_client->is_fixed_size_) {
    memcpy(ros_response, user_payload, chunk_header->userPayloadSize());
    iceoryx_client->releaseResponse(user_payload);
    *taken = true;
    ret = RMW_RET_OK;
  //} else {
    // rmw_iceoryx_cpp::deserialize(
    //   static_cast<const char *>(user_payload),
    //   &iceoryx_client->type_supports_,
    //   ros_message);
    iceoryx_client->releaseResponse(user_payload);
    *taken = true;
    ret = RMW_RET_OK;
  //}

  *taken = false;
  return ret;
}

rmw_ret_t
rmw_send_response(
  const rmw_service_t * service,
  rmw_request_id_t * request_header,
  void * ros_response)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(service, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(request_header, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(ros_response, RMW_RET_INVALID_ARGUMENT);


  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_send_response
    : service, service->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  rmw_ret_t ret = RMW_RET_ERROR;


  return ret;
}
}  // extern "C"
