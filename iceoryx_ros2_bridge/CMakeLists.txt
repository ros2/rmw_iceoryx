cmake_minimum_required(VERSION 3.5)
project(iceoryx_ros2_bridge)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(iceoryx_posh REQUIRED)
find_package(iceoryx_utils REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rcpputils REQUIRED)
find_package(rmw_iceoryx_cpp REQUIRED)
find_package(rosidl_generator_cpp REQUIRED)
find_package(rosidl_typesupport_cpp REQUIRED)
find_package(rosidl_typesupport_introspection_cpp REQUIRED)

add_library(iceoryx_typesupport_helpers
  src/typesupport_helpers.cpp
)
ament_target_dependencies(iceoryx_typesupport_helpers
  ament_index_cpp
  rclcpp
  rcpputils
  rosidl_generator_cpp
  rosidl_typesupport_introspection_cpp
  rosidl_typesupport_cpp
)

add_library(iceoryx_generic_transport
  src/generic_publisher.cpp
  src/generic_subscription.cpp
)
target_link_libraries(iceoryx_generic_transport
  iceoryx_typesupport_helpers
)
ament_target_dependencies(iceoryx_generic_transport
  rclcpp
)

add_executable(iceoryx_ros2_bridge
  src/iceoryx_ros2_bridge.cpp
)
target_link_libraries(iceoryx_ros2_bridge
  iceoryx_generic_transport
  iceoryx_posh::iceoryx_posh
)
ament_target_dependencies(iceoryx_ros2_bridge
  rclcpp
  rmw_iceoryx_cpp
)
install(
  TARGETS iceoryx_ros2_bridge
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
