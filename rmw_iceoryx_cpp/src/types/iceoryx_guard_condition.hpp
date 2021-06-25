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

#ifndef TYPES__ICEORYX_GUARD_CONDITION_HPP_
#define TYPES__ICEORYX_GUARD_CONDITION_HPP_

#include <atomic>
#include <mutex>

#include "iceoryx_utils/posix_wrapper/semaphore.hpp"

/// @todo remove this files and replace occurences with UserTrigger
class IceoryxGuardCondition
{
public:
  IceoryxGuardCondition()
  : semaphore_(nullptr), triggered_(false) {}

  void
  trigger()
  {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (semaphore_ != nullptr) {
        semaphore_->post().or_else([](auto&){

        });
      }
    }

    triggered_.store(true, std::memory_order_relaxed);
  }

  void
  attachSemaphore(iox::posix::Semaphore * const semaphore)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    semaphore_ = semaphore;
  }

  void
  detachSemaphore()
  {
    std::lock_guard<std::mutex> lock(mutex_);
    semaphore_ = nullptr;
  }

  bool
  hasTriggered()
  {
    return triggered_.load(std::memory_order_relaxed);
  }

  void resetTriggerIndication()
  {
    triggered_.store(false, std::memory_order_relaxed);
  }

private:
  std::mutex mutex_;
  iox::posix::Semaphore * semaphore_;
  std::atomic_bool triggered_;
};

#endif  // TYPES__ICEORYX_GUARD_CONDITION_HPP_
