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

#include "./types/iceoryx_client.hpp"
#include "./types/iceoryx_server.hpp"

#include "rmw_iceoryx_cpp/iceoryx_serialize.hpp"
#include "rmw_iceoryx_cpp/iceoryx_deserialize.hpp"

extern "C"
{
rmw_ret_t
rmw_send_request(
  const rmw_client_t * client,
  const void * ros_request,
  int64_t * sequence_id)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(ros_request, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(sequence_id, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_send_request
    : client,
    client->implementation_identifier,
    rmw_get_implementation_identifier(),
    return RMW_RET_ERROR);

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

  iceoryx_client->loan(iceoryx_client_abstraction->request_size_, iceoryx_client_abstraction->request_alignment_)
      .and_then([&](void * requestPayload) {
          auto requestHeader = iox::popo::RequestHeader::fromPayload(requestPayload);

          requestHeader->setSequenceId(iceoryx_client_abstraction->sequence_id_);
          *sequence_id = iceoryx_client_abstraction->sequence_id_;
          iceoryx_client_abstraction->sequence_id_ += 1;

          if (iceoryx_client_abstraction->is_fixed_size_)
          {
            memcpy(requestPayload, ros_request, iceoryx_client_abstraction->request_size_);
          }
          else
          {
            // message is not fixed size, so we have to serialize
            std::vector<char> payload_vector{};
            rmw_iceoryx_cpp::serializeRequest(ros_request, &iceoryx_client_abstraction->type_supports_, payload_vector);
            memcpy(requestPayload, payload_vector.data(), payload_vector.size());
          }
          std::cout << "Client sent request!" << std::endl;
          iceoryx_client->send(requestPayload).or_else(
              [&](auto&) {
                RMW_SET_ERROR_MSG("rmw_send_request error!");
                ret = RMW_RET_ERROR;
              });
      })
      .or_else([&](auto&) {
        RMW_SET_ERROR_MSG("rmw_send_request error!");
        ret = RMW_RET_ERROR;
      });

  ret = RMW_RET_OK;
  return ret;
}

rmw_ret_t
rmw_take_request(
  const rmw_service_t * service,
  rmw_service_info_t * request_header,
  void * ros_request,
  bool * taken)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(service, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(request_header, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(ros_request, RMW_RET_ERROR);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_ERROR);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_take_request
    : service, service->implementation_identifier,
    rmw_get_implementation_identifier(), return RMW_RET_ERROR);

  auto iceoryx_service_abstraction = static_cast<IceoryxServer *>(service->data);
  if (!iceoryx_service_abstraction) {
    RMW_SET_ERROR_MSG("service data is null");
    return RMW_RET_ERROR;
  }

  auto iceoryx_server = iceoryx_service_abstraction->iceoryx_server_;
  if (!iceoryx_server) {
    RMW_SET_ERROR_MSG("iceoryx_server is null");
    return RMW_RET_ERROR;
  }

  // this should never happen if checked already at rmw_create_service
  if (!rmw_iceoryx_cpp::iceoryx_is_valid_type_support(&iceoryx_service_abstraction->type_supports_)) {
    RMW_SET_ERROR_MSG("Use either C typesupport or CPP typesupport");
    return RMW_RET_ERROR;
  }

  const iox::mepoo::ChunkHeader * chunk_header = nullptr;
  const iox::popo::RequestHeader * iceoryx_request_header = nullptr;
  const void * user_payload = nullptr;

  rmw_ret_t ret = RMW_RET_ERROR;

  iceoryx_server->take()
  .and_then(
    [&](const void * requestPayload) {
      user_payload = requestPayload;
      chunk_header = iox::mepoo::ChunkHeader::fromUserPayload(user_payload);
      iceoryx_request_header = iox::popo::RequestHeader::fromPayload(user_payload);
      ret = RMW_RET_OK;
    })
  .or_else(
    [&](iox::popo::ServerRequestResult& error) {
      std::cout << "Could not take request! Error: " << error << std::endl;
      RMW_SET_ERROR_MSG("rmw_take_request error!");
      ret = RMW_RET_ERROR;
    });

  if (ret == RMW_RET_ERROR) {
    return ret;
  }

  // if fixed size, we fetch the data via memcpy
  if (iceoryx_service_abstraction->is_fixed_size_) {
    memcpy(ros_request, user_payload, chunk_header->userPayloadSize());
  } else {
    rmw_iceoryx_cpp::deserialize(
      static_cast<const char *>(user_payload),
      &iceoryx_service_abstraction->type_supports_,
      ros_request);
  }
  iceoryx_server->releaseRequest(user_payload);
  *taken = true;
  ret = RMW_RET_OK;
  std::cout << "Server took request!" << std::endl;

  return ret;
}
}  // extern "C"
