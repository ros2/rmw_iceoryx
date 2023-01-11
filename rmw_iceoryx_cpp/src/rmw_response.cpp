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

#include "rcutils/error_handling.h"

#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

#include "rmw_iceoryx_cpp/iceoryx_serialize.hpp"
#include "rmw_iceoryx_cpp/iceoryx_deserialize.hpp"

#include "./types/iceoryx_client.hpp"
#include "./types/iceoryx_server.hpp"

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

  auto iceoryx_client_abstraction = static_cast<IceoryxClient *>(client->data);
  if (!iceoryx_client_abstraction) {
    RMW_SET_ERROR_MSG("client data is null");
    return RMW_RET_ERROR;
  }

  auto iceoryx_client = iceoryx_client_abstraction->iceoryx_client_;
  if (!iceoryx_client) {
    RMW_SET_ERROR_MSG("iceoryx_client is null");
    return RMW_RET_ERROR;
  }

  const iox::mepoo::ChunkHeader * chunk_header = nullptr;
  const void * user_payload = nullptr;
  rmw_ret_t ret = RMW_RET_ERROR;

  iceoryx_client->take()
  .and_then(
    [&](const void * iceoryx_response_payload) {
      auto iceoryx_response_header = iox::popo::ResponseHeader::fromPayload(
        iceoryx_response_payload);

      if (iceoryx_response_header->getSequenceId() ==
      iceoryx_client_abstraction->sequence_id_ - 1)
      {
        user_payload = iceoryx_response_payload;
        chunk_header = iox::mepoo::ChunkHeader::fromUserPayload(user_payload);

        auto typed_guid = chunk_header->originId();
        iox::popo::UniquePortId::value_type guid =
          static_cast<iox::popo::UniquePortId::value_type>(typed_guid);
        size_t size = sizeof(guid);
        auto max_rmw_storage = sizeof(request_header->request_id.writer_guid);
        if (!typed_guid.isValid() || size > max_rmw_storage) {
          RMW_SET_ERROR_MSG("Could not write server guid");
          ret = RMW_RET_ERROR;
          return;
        }
        memcpy(request_header->request_id.writer_guid, &guid, size);
        request_header->request_id.sequence_number = iceoryx_response_header->getSequenceId();
        request_header->source_timestamp = 0;  // Unsupported until needed
        ret = rcutils_system_time_now(&request_header->received_timestamp);
        if (ret != RMW_RET_OK) {
          return;
        }
        ret = RMW_RET_OK;
      } else {
        RMW_SET_ERROR_MSG("Got response with outdated sequence number!");
        *taken = false;
        ret = RMW_RET_ERROR;
      }
    })
  .or_else(
    [&](iox::popo::ChunkReceiveResult) {
      *taken = false;
      RMW_SET_ERROR_MSG("No chunk in iceoryx_client");
      ret = RMW_RET_ERROR;
    });

  if (ret == RMW_RET_ERROR) {
    return ret;
  }

  // if fixed size, we fetch the data via memcpy
  if (iceoryx_client_abstraction->is_fixed_size_) {
    memcpy(ros_response, user_payload, chunk_header->userPayloadSize());
  } else {
    rmw_iceoryx_cpp::deserializeResponse(
      static_cast<const char *>(user_payload),
      &iceoryx_client_abstraction->type_supports_,
      ros_response);
  }
  iceoryx_client->releaseResponse(user_payload);
  *taken = true;
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

  auto iceoryx_server_abstraction = static_cast<IceoryxServer *>(service->data);
  if (!iceoryx_server_abstraction) {
    RMW_SET_ERROR_MSG("service data is null");
    return RMW_RET_ERROR;
  }

  auto iceoryx_server = iceoryx_server_abstraction->iceoryx_server_;
  if (!iceoryx_server) {
    RMW_SET_ERROR_MSG("iceoryx_server is null");
    return RMW_RET_ERROR;
  }

  rmw_ret_t ret = RMW_RET_ERROR;

  if (!iceoryx_server_abstraction->request_payload_) {
    RMW_SET_ERROR_MSG("'rmw_take_request' needs to be called before 'rmw_send_response'!");
    ret = RMW_RET_ERROR;
    return ret;
  }

  auto * iceoryx_request_header = iox::popo::RequestHeader::fromPayload(
    iceoryx_server_abstraction->request_payload_);

  iceoryx_server->loan(
    iceoryx_request_header, iceoryx_server_abstraction->response_size_,
    iceoryx_server_abstraction->response_alignment_)
  .and_then(
    [&](void * responsePayload) {
      if (iceoryx_server_abstraction->is_fixed_size_) {
        memcpy(responsePayload, ros_response, iceoryx_server_abstraction->response_size_);
      } else {
        // message is not fixed size, so we have to serialize
        std::vector<char> payload_vector{};
        rmw_iceoryx_cpp::serializeResponse(
          ros_response,
          &iceoryx_server_abstraction->type_supports_, payload_vector);
        memcpy(responsePayload, payload_vector.data(), payload_vector.size());
      }
      /// @todo Why are the sleeps before and after 'send()' needed?
      /// rmw_cyclonedds and rmw_fastrtps seem to do something similar in 'rmw_send_response'..
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      iceoryx_server->send(responsePayload).and_then(
        [&] {
          ret = RMW_RET_OK;
        }).or_else(
        [&](auto &) {
          RMW_SET_ERROR_MSG("rmw_send_response send error!");
          ret = RMW_RET_ERROR;
        });
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    })
  .or_else(
    [&](auto & error) {
      RMW_SET_ERROR_MSG("rmw_send_response loan error!");
      ret = RMW_RET_ERROR;
    });

  // Release the hold request
  iceoryx_server->releaseRequest(iceoryx_server_abstraction->request_payload_);
  iceoryx_server_abstraction->request_payload_ = nullptr;

  return ret;
}
}  // extern "C"
