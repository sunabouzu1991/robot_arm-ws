cmake_minimum_required(VERSION 3.8)
project(arm_control)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(moveit_ros_planning_interface REQUIRED)
find_package(geometry_msgs REQUIRED)

# Слои собираются отдельными целями: нарушение направления зависимостей
# (domain -> adapters) ломается на сборке, а не на ревью.
# Разрешённое направление: apps -> adapters -> application -> domain.

# --- domain: чистая логика, без ROS и MoveIt --------------------------------
add_library(arm_control_domain STATIC
  src/domain/joint_target.cpp
  src/domain/motion_outcome.cpp
  src/domain/motion_settings.cpp
  src/domain/motion_task.cpp
  src/domain/tool_pose.cpp
)
target_include_directories(arm_control_domain PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# --- application: политика выполнения команд, только domain + ports ---------
add_library(arm_control_application STATIC
  src/application/motion_task_runner.cpp
)
target_link_libraries(arm_control_application arm_control_domain)

# --- adapters: единственное место, знающее о MoveIt и rclcpp ----------------
add_library(arm_control_moveit_adapter STATIC
  src/adapters/motion_settings_parameters.cpp
  src/adapters/moveit_motion_planner.cpp
  src/adapters/moveit_session.cpp
  src/adapters/outcome_report.cpp
)
target_link_libraries(arm_control_moveit_adapter arm_control_domain)
ament_target_dependencies(arm_control_moveit_adapter
  rclcpp
  moveit_ros_planning_interface
  geometry_msgs
)

# --- apps: только композиция слоёв ------------------------------------------
foreach(app move_joints move_tool_pose)
  add_executable(${app} apps/${app}.cpp)
  target_link_libraries(${app}
    arm_control_application
    arm_control_moveit_adapter
  )
  ament_target_dependencies(${app}
    rclcpp
    moveit_ros_planning_interface
    geometry_msgs
  )
endforeach()

install(TARGETS
  arm_control_domain
  arm_control_application
  arm_control_moveit_adapter
  ARCHIVE DESTINATION lib
)
install(TARGETS
  move_joints
  move_tool_pose
  DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY config DESTINATION share/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_tool_pose test/domain/test_tool_pose.cpp)
  target_link_libraries(test_tool_pose arm_control_domain)

  ament_add_gtest(test_joint_target test/domain/test_joint_target.cpp)
  target_link_libraries(test_joint_target arm_control_domain)

  ament_add_gtest(test_motion_task test/domain/test_motion_task.cpp)
  target_link_libraries(test_motion_task arm_control_domain)

  ament_add_gtest(test_motion_task_runner test/application/test_motion_task_runner.cpp)
  target_link_libraries(test_motion_task_runner arm_control_application)
endif()

ament_package()
