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

extern "C"
{
namespace details
{
/// @todo move this to the method as we need to store the ptr to the payload

rmw_ret_t
take_request(
  iox::popo::UntypedServer * iceoryx_server,
  const void * serialized_ros_msg,
  size_t size)
{
  if (serialized_ros_msg == nullptr) {
    RMW_SET_ERROR_MSG("serialized message pointer is null");
    return RMW_RET_ERROR;
  }
  rmw_ret_t ret = RMW_RET_ERROR;
  iceoryx_server->take()
  .and_then(
    [&](auto& requestPayload) {
      // memcpy(userPayload, serialized_ros_msg, size);
      // iceoryx_server->publish(userPayload);

      // Hier die calculate response Methode aufrufen?
      // nope eher sample abspeichern im struct
      ret = RMW_RET_OK;
    })
  .or_else(
    [&](auto&) {
      RMW_SET_ERROR_MSG("take_request error!");
      ret = RMW_RET_ERROR;
    });
  return ret;
}
}

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

  iceoryx_client->loan(iceoryx_client_abstraction->message_size_, iceoryx_client_abstraction->message_alignment_)
      .and_then([&](auto& requestPayload) {
          auto requestHeader = iox::popo::RequestHeader::fromPayload(requestPayload);
          requestHeader->setSequenceId(*sequence_id);
          /// @todo memcpy or serialize the response
          void * request;
          // auto request = static_cast<AddRequest*>(requestPayload);
          // request->augend = fibonacciLast;
          // request->addend = fibonacciCurrent;
          iceoryx_client->send(request).or_else(
              [&](auto& error) { ret = RMW_RET_ERROR; });
      })
      .or_else([&](auto& error) { ret = RMW_RET_ERROR; });

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

  // if messages have a fixed size, we can just memcpy
  if (iceoryx_service_abstraction->is_fixed_size_) {
    return details::take_request(iceoryx_server, request_header, iceoryx_service_abstraction->message_size_);
  }

  // this should never happen if checked already at rmw_create_service
  if (!rmw_iceoryx_cpp::iceoryx_is_valid_type_support(&iceoryx_service_abstraction->type_supports_)) {
    RMW_SET_ERROR_MSG("Use either C typesupport or CPP typesupport");
    return RMW_RET_ERROR;
  }

  // // message is neither loaned nor fixed size, so we have to serialize
  // std::vector<char> payload_vector{};

  // rmw_iceoryx_cpp::serialize(ros_message, &iceoryx_publisher->type_supports_, payload_vector);

  // // send composed payload
  // return details::send_payload(iceoryx_sender, payload_vector.data(), payload_vector.size());

  *taken = true;
  return RMW_RET_OK;
}
}  // extern "C"
