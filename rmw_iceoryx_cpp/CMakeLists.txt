cmake_minimum_required(VERSION 3.5)
project(rmw_iceoryx_cpp)

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

if(CODE_COVERAGE AND
  CMAKE_SYSTEM_NAME MATCHES "Linux" AND
  CMAKE_C_COMPILER_ID STREQUAL "GNU")
  message("GCov code coverage enabled")
  set(CMAKE_CXX_FLAGS "--coverage")
  set(CMAKE_EXE_LINKER_FLAGS "--coverage")
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(iceoryx_posh QUIET)
if(NOT iceoryx_posh_FOUND)
  ament_package()
  message(WARNING "iceoryx not found - skipping '${PROJECT_NAME}'")
  return()
endif()
find_package(rcpputils REQUIRED)
find_package(rcutils REQUIRED)
find_package(rmw REQUIRED)
find_package(rosidl_runtime_c REQUIRED)
find_package(rosidl_runtime_cpp REQUIRED)
find_package(rosidl_typesupport_cpp REQUIRED)
find_package(rosidl_typesupport_introspection_c REQUIRED)
find_package(rosidl_typesupport_introspection_cpp REQUIRED)

add_library(rmw_iceoryx_serialization SHARED
  src/internal/iceoryx_deserialize.cpp
  src/internal/iceoryx_serialize.cpp
)
target_include_directories(rmw_iceoryx_serialization
  PUBLIC include
)
ament_target_dependencies(rmw_iceoryx_serialization
  rosidl_runtime_c
  rosidl_runtime_cpp
  rosidl_typesupport_introspection_c
  rosidl_typesupport_introspection_cpp
)
add_library(rmw_iceoryx_name_conversion SHARED
  src/internal/iceoryx_name_conversion.cpp
  src/internal/iceoryx_type_info_introspection.cpp
  src/internal/iceoryx_topic_names_and_types.cpp
  src/internal/iceoryx_get_topic_endpoint_info.cpp
)
target_include_directories(rmw_iceoryx_name_conversion
  PUBLIC include
)
target_link_libraries(rmw_iceoryx_name_conversion
  iceoryx_posh::iceoryx_posh
)
ament_target_dependencies(rmw_iceoryx_name_conversion
  rcpputils
  rcutils
  rmw
  rosidl_runtime_c
  rosidl_runtime_cpp
  rosidl_typesupport_introspection_c
  rosidl_typesupport_introspection_cpp
)
add_library(rmw_iceoryx_cpp SHARED
  src/internal/iceoryx_generate_gid.cpp
  src/rmw_client.cpp
  src/rmw_compare_guids_equal.cpp
  src/rmw_count.cpp
  src/rmw_event.cpp
  src/rmw_get_gid_for_publisher.cpp
  src/rmw_get_implementation_identifier.cpp
  src/rmw_get_serialization_format.cpp
  src/rmw_get_topic_endpoint_info.cpp
  src/rmw_guard_condition.cpp
  src/rmw_init.cpp
  src/rmw_logging.cpp
  src/rmw_network_flow_endpoint.cpp
  src/rmw_node.cpp
  src/rmw_node_info_and_types.cpp
  src/rmw_node_names.cpp
  src/rmw_publish.cpp
  src/rmw_publisher.cpp
  src/rmw_qos.cpp
  src/rmw_request.cpp
  src/rmw_response.cpp
  src/rmw_serialize.cpp
  src/rmw_service.cpp
  src/rmw_service_names_and_types.cpp
  src/rmw_service_server_is_available.cpp
  src/rmw_subscription.cpp
  src/rmw_take.cpp
  src/rmw_topic_names_and_types.cpp
  src/rmw_trigger_guard_condition.cpp
  src/rmw_wait.cpp
  src/rmw_wait_set.cpp
)
ament_target_dependencies(rmw_iceoryx_cpp
  "rcutils"
  "rmw"
  "rosidl_typesupport_introspection_cpp"
)
target_include_directories(
  rmw_iceoryx_cpp
  PUBLIC include
)
target_link_libraries(rmw_iceoryx_cpp
  rmw_iceoryx_serialization
  rmw_iceoryx_name_conversion
  iceoryx_posh::iceoryx_posh
)

register_rmw_implementation(
  "c:rosidl_typesupport_c:rosidl_typesupport_introspection_c"
  "cpp:rosidl_typesupport_cpp:rosidl_typesupport_introspection_cpp"
)
configure_rmw_library(rmw_iceoryx_cpp)

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  TARGETS rmw_iceoryx_serialization rmw_iceoryx_name_conversion rmw_iceoryx_cpp
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(ament_cmake_gtest REQUIRED)
  find_package(test_msgs REQUIRED)

  ament_add_gtest(test_name_conversion test/iceoryx_name_conversion_test.cpp)
  target_link_libraries(test_name_conversion ${PROJECT_NAME})

  ament_add_gtest(test_message_serialization test/iceoryx_serialization_test.cpp)
  target_link_libraries(test_message_serialization ${PROJECT_NAME})
  ament_target_dependencies(test_message_serialization
    test_msgs
  )

  ament_add_gtest(test_fixed_size_messages test/iceoryx_fixed_size_messages_test.cpp)
  target_link_libraries(test_fixed_size_messages ${PROJECT_NAME})
  ament_target_dependencies(test_fixed_size_messages
    test_msgs
  )
endif()

ament_export_include_directories(include)
ament_export_libraries(rmw_iceoryx_serialization rmw_iceoryx_name_conversion rmw_iceoryx_cpp)
ament_package()
