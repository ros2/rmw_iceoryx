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

#ifndef TYPES__ICEORYX_WAIT_SET_HPP_
#define TYPES__ICEORYX_WAIT_SET_HPP_

#include "iceoryx_posh/popo/subscriber.hpp"
#include "iceoryx_utils/posix_wrapper/semaphore.hpp"

#include "rmw/rmw.h"
#include "rmw/types.h"

struct IceoryxWaitSet
{
  IceoryxWaitSet(
    iox::posix::Semaphore * const semaphore,
    iox::popo::Subscriber * const iceoryx_receiver)
  : semaphore_(semaphore), iceoryx_receiver_(iceoryx_receiver)
  {}

  iox::posix::Semaphore * const semaphore_;
  iox::popo::Subscriber * const iceoryx_receiver_;
};

#endif  // TYPES__ICEORYX_WAIT_SET_HPP_
