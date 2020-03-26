![Integration build and tests](https://github.com/ros2/rmw_iceoryx/workflows/Integration%20build%20rmw_iceoryx/badge.svg)
![Linting](https://github.com/ros2/rmw_iceoryx/workflows/Lint%20rmw_iceoryx/badge.svg)
![Iceoryx build and tests](https://github.com/ros2/rmw_iceoryx/workflows/Build%20iceoryx/badge.svg)

Installation
============

The following instructions show you how to install [iceoryx](https://github.com/eclipse/iceoryx) and its rmw implementation.
The installation of iceoryx and rmw_iceoryx is pretty straight forward.
All provided packages can be built with colcon so that you can easily build both, iceoryx and rmw_iceoryx, within your ROS2 workspace.
rmw_iceoryx is using the [rosidl_typesupport_introspection](https://github.com/ros2/rosidl) which allows for building iceoryx on top of an existing ROS2 workspace or even debian installation as no ROS2 messages have to be built again.

To install iceoryx and rmw_iceoryx in a ROS2 workspace, just execute the steps below:

```
mkdir -p ~/iceoryx_ws/src
cd $_
git clone https://github.com/eclipse/iceoryx.git
git clone https://github.com/ros2/rmw_iceoryx.git
```
For alternative installation instructions and more details about iceoryx's internals, please see [iceoryx's GitHub repo](https://github.com/eclipse/iceoryx).

rmw_iceoryx is compatible with ROS2 Eloquent.
Assuming you have ROS2 installed correctly, you can compile the iceoryx workspace with colcon:

```
cd ~/iceoryx_ws/
source /opt/ros/eloquent/setup.bash  # alternatively source your own ROS2 workspace
colcon build
```

That's it! You've installed iceoryx and are ready to rumble.

Working with rmw_iceoryx_cpp
============================

Getting Started
---------------

iceoryx is based on shared memory and features a shared memory management application called RouDi.
RouDi is a daemon taking care of allocating enough space within the shared memory each node and is responsible for transporting messages between these nodes.

Before starting any iceoryx application, we therefore have to start the daemon.

```
./iceoryx_ws/install/iceoryx_posh/bin/RouDi  # /iceoryx_ws/install/bin/RouDi if you installed with as a merged workspace
```

You can then use rmw_iceoryx_cpp just like any other available rmw implementation.
In order to specify the rmw implementation, you have to set the environment variable `RMW_IMPLEMENTATION` to `rmw_iceoryx_cpp`.

To run the ROS2 c++ demo nodes with iceoryx, you can thus execute the following command:

```
source ~/iceoryx_ws/install/setup.bash
RMW_IMPLEMENTATION=rmw_iceoryx_cpp ros2 run demo_nodes_cpp talker
```

In another terminal, you can then subscribe to the talker as always:
```
source ~/iceoryx_ws/install/setup.bash
RMW_IMPLEMENTATION=rmw_iceoryx_cpp ros2 run demo_nodes_cpp listener
```

Zero Copy - The True Power
--------------------------

To exploit iceoryx's full potential, we want to leverage the zero copy transport mechanism it provides.
For this to work, we have to take one step back and look at the details of what it means to enable zero copy data transport.

The basic zero copy workflow works as depicted in the picture below:
![](docs/ros2_loan_messages.png)

Step 1 `loan_message()`) A publisher asks rmw_iceoryx_cpp to loan a message from it.
rmw_iceoryx_cpp allocates the appropriate message in its shared memory and loans it to the publisher.

Step 2 `publish()`) The publisher can fill in the data into the loaned message.
When calling publish, the loaned message will be returned to the middleware and the publisher has no longer ownership over the message.

Step 3 `take_loaned_message()`) A subscription wants to take a message from the middleware.
rmw_iceoryx_cpp gives a loaned message to the subscription which can execute their respective callbacks.

Step 4 `return_loaned_message()`) A subscription callback is finished and the loaned message is getting returned to the middleware.

Starting from ROS2 Eloquent, these features are implemented within rclcpp.
An application using these new features is shown in the code snippet below.
For a fully working implementation, please have a look at [this demo node](https://github.com/ros2/demos/blob/master/demo_nodes_cpp/src/topics/talker_loaned_message.cpp).

```c++
auto pub = node->create_publisher<std_msgs::msg::String>("/chatter", 1);
// Ask the publisher to loan a String message
auto loaned_msg = pub_->borrow_loaned_message();
pod_loaned_msg.get().data = "Hello World";
// Return ownership of that string message
pod_pub_->publish(std::move(pod_loaned_msg));
```

The code above has one problem though: How can the middleware allocate enough memory for the string message?
The middleware can't possibly know the size of the string the user wants to put into that message.

That being said, in order to enable a true zero copy data transport we have to limit ourselves to fixed size data structures.
The picture below tries to illustrate the difference between a fixed size message and a dynamically resizable message.
![](docs/fixed_size_messages.png)

The plain old datatype (POD) on the left side is very well suited for zero copy data transport as its size is definitely defined (on compile time).
The message definition shown on the right size is not made for zero copy transport as its size might vary during runtime and rmw_iceoryx_cpp can't determine how much memory should be allocated in the shared memory.

Thus, in order to make our demo work with zero copy, we can alternatively send a float64, as its size is clearly defined.

```c++
auto pub = node->create_publisher<std_msgs::msg::Float64>("/float", 1);
// Ask the publisher to loan a Float64 message
auto loaned_msg = pub_->borrow_loaned_message();
pod_loaned_msg.get().data = 123.456f;
// Return ownership of that Float64 message
pod_pub_->publish(std::move(pod_loaned_msg));
```

If you'd like to play around with the zero copy transport, we recommend to checkout the [fixed size image transport demo](https://github.com/karsten1987/fixed_size_ros2_demo), which illustrates how iceoryx can be used to publish and subscribe up to even 4K images without having to copy them.

Limitations
===========

rmw_iceoryx_cpp is currently under heavy development.
Unfortunately, not all features are yet fully fleshed out.
Other core functionalities like e.g. services are not yet implemented, but will follow soon.
