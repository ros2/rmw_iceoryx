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

#ifndef TYPES__ICEORYX_CLIENT_HPP_
#define TYPES__ICEORYX_CLIENT_HPP_

#include "iceoryx_posh/popo/untyped_client.hpp"

#include "rmw/rmw.h"
#include "rmw/types.h"

#include "rmw_iceoryx_cpp/iceoryx_type_info_introspection.hpp"

struct IceoryxClient
{
  IceoryxClient(
    const rosidl_service_type_support_t * type_supports,
    iox::popo::UntypedClient * const iceoryx_client)
  : type_supports_(*type_supports),
    iceoryx_client_(iceoryx_client),
    is_fixed_size_(rmw_iceoryx_cpp::iceoryx_is_fixed_size(type_supports)),
    request_size_(rmw_iceoryx_cpp::iceoryx_get_request_size(type_supports))
  {}

  rosidl_service_type_support_t type_supports_;
  iox::popo::UntypedClient * const iceoryx_client_;
  bool is_fixed_size_;
  size_t request_size_;
  int64_t sequence_id_;
};

#endif  // TYPES__ICEORYX_CLIENT_HPP_
