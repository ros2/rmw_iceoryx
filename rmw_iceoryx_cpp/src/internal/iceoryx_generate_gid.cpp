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

#include "rmw/rmw.h"
#include "rmw/types.h"

rmw_gid_t generate_gid()
{
  rmw_gid_t gid;
  // TODO(mphnl) What would be the GUID for a sender in iceoryx?
  gid.implementation_identifier = rmw_get_implementation_identifier();
  memset(&gid.data[0], 0, RMW_GID_STORAGE_SIZE);

  static size_t dummy_guid = 0;
  dummy_guid++;
  memcpy(&gid.data[0], &dummy_guid, sizeof(size_t));

  return gid;
}
