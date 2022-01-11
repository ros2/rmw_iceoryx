// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2022 by Apex.AI Inc. All rights reserved.
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

#include "rmw/rmw.h"
#include "rmw/types.h"
#include "rmw/impl/cpp/macros.hpp"

#include "iceoryx_posh/popo/untyped_publisher.hpp"

rmw_gid_t generate_publisher_gid(iox::popo::UntypedPublisher * const publisher)
{
  rmw_gid_t gid;
  gid.implementation_identifier = rmw_get_implementation_identifier();
  memset(gid.data, 0, RMW_GID_STORAGE_SIZE);

  iox::UniquePortId typed_uid = publisher->getUid();
  iox::UniquePortId::value_type uid = static_cast<iox::UniquePortId::value_type>(typed_uid);
  size_t size = sizeof(uid);

  if (!typed_uid.isValid() || size > RMW_GID_STORAGE_SIZE) {
    RMW_SET_ERROR_MSG("Could not generated gid");
    return gid;
  }
  memcpy(gid.data, &uid, size);

  return gid;
}
