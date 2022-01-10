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

#include "./types/iceoryx_publisher.hpp"

extern "C"
{
rmw_ret_t
rmw_get_gid_for_publisher(const rmw_publisher_t * publisher, rmw_gid_t * gid)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(gid, RMW_RET_INVALID_ARGUMENT);

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_get_gid_for_publisher
    : publisher, publisher->implementation_identifier,
    rmw_get_implementation_identifier(),
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  IceoryxPublisher * iceoryx_publisher = static_cast<IceoryxPublisher *>(publisher->data);
  if (!iceoryx_publisher) {
    RMW_SET_ERROR_MSG("publisher info handle is null");
    return RMW_RET_INVALID_ARGUMENT;
  }
  *gid = iceoryx_publisher->gid_;
  return RMW_RET_OK;
}
}  // extern "C"
