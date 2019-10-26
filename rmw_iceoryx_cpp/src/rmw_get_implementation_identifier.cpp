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

#include "./iceoryx_identifier.hpp"

const char * const rmw_iceoryx_cpp_identifier = "rmw_iceoryx_cpp";

extern "C"
{
const char *
rmw_get_implementation_identifier()
{
  return rmw_iceoryx_cpp_identifier;
}
}  // extern "C"
