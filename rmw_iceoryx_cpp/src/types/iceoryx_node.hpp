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

#ifndef TYPES__ICEORYX_NODE_HPP_
#define TYPES__ICEORYX_NODE_HPP_

#include <functional>

#include "iceoryx_posh/popo/subscriber.hpp"
#include "iceoryx_posh/roudi/introspection_types.hpp"
#include "iceoryx_posh/runtime/runnable.hpp"

#include "rcutils/error_handling.h"

#include "rmw/impl/cpp/macros.hpp"
#include "rmw/rmw.h"

#include "../iceoryx_identifier.hpp"
#include "./iceoryx_guard_condition.hpp"

// We currently use the iceoryx port introspection
// which is updated whenever a sender port comes or goes or
// does OFFER / STOP_OFFER or a receiver port comes or goes or does SUB / UNSUB
// TODO(mphnl) check with the list of ros2 graph events

class IceoryxGraphChangeNotifier
{
public:
  explicit IceoryxGraphChangeNotifier(rmw_guard_condition_t * guard_condition)
  {
    if (!guard_condition || !guard_condition->data) {
      RMW_SET_ERROR_MSG("invalid input for GraphChangeNotifier");
      iceoryx_guard_condition_ = nullptr;
    } else {
      iceoryx_guard_condition_ = static_cast<IceoryxGuardCondition *>(guard_condition->data);
      RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
        IceoryxGraphChangeNotifier
        : guard_condition,
        guard_condition->implementation_identifier,
        rmw_get_implementation_identifier(),
        iceoryx_guard_condition_ = nullptr);
    }

    // subscribe with a callback for changes in the iceoryx graph
    port_receiver_.setReceiveHandler(std::bind(&IceoryxGraphChangeNotifier::callback, this));
    port_receiver_.subscribe(1);
  }

  ~IceoryxGraphChangeNotifier()
  {
    port_receiver_.unsetReceiveHandler();
    port_receiver_.unsubscribe();
  }

private:
  void callback()
  {
    if (nullptr != iceoryx_guard_condition_) {
      iceoryx_guard_condition_->trigger();
    }
  }

  IceoryxGuardCondition * iceoryx_guard_condition_{nullptr};
  using port_receiver_t = iox::popo::Subscriber;
  port_receiver_t port_receiver_{iox::roudi::IntrospectionPortService};
};

struct IceoryxNodeInfo
{
  IceoryxNodeInfo(
    rmw_guard_condition_t * guard_condition,
    IceoryxGraphChangeNotifier * graph_change_notifier,
    iox::runtime::Runnable * iceoryx_runnable)
  : guard_condition_(guard_condition),
    graph_change_notifier_(graph_change_notifier),
    iceoryx_runnable_(iceoryx_runnable)
  {
  }
  rmw_guard_condition_t * const guard_condition_;
  IceoryxGraphChangeNotifier * const graph_change_notifier_;
  iox::runtime::Runnable * const iceoryx_runnable_;
};

#endif  // TYPES__ICEORYX_NODE_HPP_
