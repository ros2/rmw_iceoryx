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

  iceoryx_client->loan(
    iceoryx_client_abstraction->request_size_,
    iox::CHUNK_DEFAULT_USER_PAYLOAD_ALIGNMENT)
  .and_then(
    [&](void * requestPayload) {
      auto requestHeader = iox::popo::RequestHeader::fromPayload(requestPayload);

      requestHeader->setSequenceId(iceoryx_client_abstraction->sequence_id_);
      *sequence_id = iceoryx_client_abstraction->sequence_id_;
      iceoryx_client_abstraction->sequence_id_ += 1;

      if (iceoryx_client_abstraction->is_fixed_size_) {
        memcpy(requestPayload, ros_request, iceoryx_client_abstraction->request_size_);
      } else {
        // message is not fixed size, so we have to serialize
        std::vector<char> payload_vector{};
        rmw_iceoryx_cpp::serializeRequest(
          ros_request, &iceoryx_client_abstraction->type_supports_,
          payload_vector);
        memcpy(requestPayload, payload_vector.data(), payload_vector.size());
      }
      iceoryx_client->send(requestPayload).and_then(
        [&] {
          ret = RMW_RET_OK;
        }).or_else(
        [&](auto &) {
          RMW_SET_ERROR_MSG("rmw_send_request error!");
          ret = RMW_RET_ERROR;
        });
    })
  .or_else(
    [&](auto &) {
      RMW_SET_ERROR_MSG("rmw_send_request error!");
      ret = RMW_RET_ERROR;
    });

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

  // this should never happen if checked already at rmw_create_service
  if (!rmw_iceoryx_cpp::iceoryx_is_valid_type_support(&iceoryx_server_abstraction->type_supports_))
  {
    RMW_SET_ERROR_MSG("Use either C typesupport or CPP typesupport");
    return RMW_RET_ERROR;
  }

  rmw_ret_t ret = RMW_RET_ERROR;

  iceoryx_server->take()
  .and_then(
    [&](const void * iceoryx_request_payload) {
      const iox::mepoo::ChunkHeader * chunk_header =
      iox::mepoo::ChunkHeader::fromUserPayload(
        iceoryx_request_payload);
      const iox::popo::RequestHeader * iceoryx_request_header =
      iox::popo::RequestHeader::fromPayload(
        iceoryx_request_payload);

      request_header->source_timestamp = 0;  // Unsupported until needed
      ret = rcutils_system_time_now(&request_header->received_timestamp);
      if (ret != RMW_RET_OK) {
        return;
      }
      request_header->request_id.sequence_number = iceoryx_request_header->getSequenceId();
      auto typed_guid = iceoryx_server->getUid();
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

      // if fixed size, we fetch the data via memcpy
      if (iceoryx_server_abstraction->is_fixed_size_) {
        memcpy(ros_request, iceoryx_request_payload, chunk_header->userPayloadSize());
      } else {
        rmw_iceoryx_cpp::deserializeRequest(
          static_cast<const char *>(iceoryx_request_payload),
          &iceoryx_server_abstraction->type_supports_,
          ros_request);
      }

      // Hold the loaned request till we send the response in 'rmw_send_response'
      auto result =
      iceoryx_server_abstraction->request_payload_map_.insert(
        {request_header->request_id.
          sequence_number, iceoryx_request_payload});

      if (result.second == false) {
        *taken = false;
        RMW_SET_ERROR_MSG("rmw_take_request: Could not store the sample pointer in the map!");
        ret = RMW_RET_ERROR;
        return;
      }

      *taken = true;
      ret = RMW_RET_OK;
    })
  .or_else(
    [&](auto &) {
      *taken = false;
      RMW_SET_ERROR_MSG("rmw_take_request: Taking the sample failed!");
      ret = RMW_RET_ERROR;
    });

  return ret;
}
}  // extern "C"
