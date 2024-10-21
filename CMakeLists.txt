cmake_minimum_required(VERSION 3.8)
project(battery_level_broadcaster)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror=unused-but-set-variable -Werror=return-type -Werror=shadow -Werror=format)
endif()

set(THIS_PACKAGE_INCLUDE_DEPENDS
    controller_interface
    generate_parameter_library
    hardware_interface
    pluginlib
    rclcpp
    rclcpp_lifecycle
    realtime_tools
    sensor_msgs
)

find_package(ament_cmake REQUIRED)
find_package(backward_ros REQUIRED)
foreach(Dependency IN ITEMS ${THIS_PACKAGE_INCLUDE_DEPENDS})
    find_package(${Dependency} REQUIRED)
endforeach()

generate_parameter_library(battery_level_broadcaster_parameters src/battery_level_broadcaster_parameters.yaml)
add_library(battery_level_broadcaster SHARED src/battery_level_broadcaster.cpp)
target_include_directories(battery_level_broadcaster PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/battery_level_broadcaster>
)

target_compile_features(battery_level_broadcaster PUBLIC cxx_std_17)
ament_target_dependencies(battery_level_broadcaster PUBLIC ${THIS_PACKAGE_INCLUDE_DEPENDS})

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(battery_level_broadcaster PRIVATE "BATTERY_LEVEL_BROADCASTER_BUILDING_DLL")
target_link_libraries(battery_level_broadcaster PUBLIC
    battery_level_broadcaster_parameters
)

pluginlib_export_plugin_description_file(controller_interface battery_level_broadcaster.xml)

install(
    DIRECTORY include/
    DESTINATION include/battery_level_broadcaster
)
install(
    TARGETS
    battery_level_broadcaster
    battery_level_broadcaster_parameters
    EXPORT export_battery_level_broadcaster
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
)

ament_export_targets(export_battery_level_broadcaster HAS_LIBRARY_TARGET)
ament_export_dependencies(${THIS_PACKAGE_INCLUDE_DEPENDS})

ament_package()