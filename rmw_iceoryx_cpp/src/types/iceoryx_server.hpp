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

#ifndef TYPES__ICEORYX_SERVER_HPP_
#define TYPES__ICEORYX_SERVER_HPP_

#include <map>

#include "iceoryx_posh/popo/untyped_server.hpp"

#include "rmw/rmw.h"
#include "rmw/types.h"

#include "rmw_iceoryx_cpp/iceoryx_type_info_introspection.hpp"

struct IceoryxServer
{
  IceoryxServer(
    const rosidl_service_type_support_t * type_supports,
    iox::popo::UntypedServer * const iceoryx_server)
  : type_supports_(*type_supports),
    iceoryx_server_(iceoryx_server),
    is_fixed_size_(rmw_iceoryx_cpp::iceoryx_is_fixed_size(type_supports)),
    response_size_(rmw_iceoryx_cpp::iceoryx_get_response_size(type_supports))
  {
  }

  rosidl_service_type_support_t type_supports_;
  iox::popo::UntypedServer * const iceoryx_server_;
  bool is_fixed_size_{false};
  size_t response_size_{0};
  /// @brief The map stores the sequence numbers together with the corresponding
  ///        sample pointer pointing to the shared memory. This is due to the fact that
  ///        'rmw_request_id_t' misses a place to store the sample pointer, which is not
  ///        typical with DDS implementations.
  std::map<int64_t, const void *> request_payload_map_;
};

#endif  // TYPES__ICEORYX_SERVER_HPP_
