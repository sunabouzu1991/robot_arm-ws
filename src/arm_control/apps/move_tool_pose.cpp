// Точка входа демонстрации: движение инструмента (tcp) в заданную 6D позу и возврат в home.
// Здесь только композиция слоёв и демонстрационные данные — логики IK нет.

#include <cstdlib>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>

#include "arm_control/adapters/motion_settings_parameters.hpp"
#include "arm_control/adapters/moveit_motion_planner.hpp"
#include "arm_control/adapters/moveit_session.hpp"
#include "arm_control/adapters/outcome_report.hpp"
#include "arm_control/application/motion_task_runner.hpp"

namespace
{
const std::string kNodeName = "move_tool_pose";

// Целевая поза получена прямой кинематикой компактной модели для конфигурации
// [0.3, 0.2, -0.3, 0.1, 0.2, 0.1], проверенной на самоколлизии.
const arm_control::domain::Vector3 kTargetPosition{0.01864480, 0.00670176, 0.49665482};
const arm_control::domain::Quaternion kTargetOrientation{
  -0.01241484, 0.04892170, 0.24660596, 0.96780062};

const arm_control::domain::Tolerances kTargetTolerances{0.005, 0.01};

std::vector<arm_control::domain::MotionTask> buildTasks(const arm_control::domain::MotionSettings& settings)
{
  const arm_control::domain::ToolPoseTask approach{
    arm_control::domain::makeToolPose(kTargetPosition, kTargetOrientation), kTargetTolerances};

  return {
    approach,
    arm_control::domain::NamedPoseTask{settings.home_pose_name},
  };
}

void logPose(const rclcpp::Logger& logger, const char* prefix, const arm_control::domain::ToolPose& pose)
{
  const arm_control::domain::Rpy rpy = arm_control::domain::toRpy(pose.orientation);
  RCLCPP_INFO(
    logger, "%s [X: %.4f, Y: %.4f, Z: %.4f | RPY: %.4f, %.4f, %.4f]", prefix, pose.position.x,
    pose.position.y, pose.position.z, rpy.roll, rpy.pitch, rpy.yaw);
}

int runDemo()
{
  arm_control::adapters::MoveItSession session{kNodeName};
  const auto settings = arm_control::adapters::readMotionSettings(*session.node());

  const std::string problem = arm_control::domain::validationError(settings);
  if (!problem.empty()) {
    RCLCPP_ERROR(session.logger(), "%s", problem.c_str());
    return EXIT_FAILURE;
  }

  arm_control::adapters::MoveItMotionPlanner planner{session.node(), settings};

  const auto current_pose = planner.currentToolPose();
  if (!current_pose) {
    RCLCPP_ERROR(session.logger(), "No joint state for link '%s' — is move_group running?", settings.tool_frame.c_str());
    return EXIT_FAILURE;
  }
  logPose(session.logger(), "Current tool pose:", *current_pose);

  const auto tasks = buildTasks(settings);
  for (const auto& task : tasks) {
    RCLCPP_INFO(session.logger(), "Requested: %s", arm_control::domain::describe(task).c_str());
  }

  const auto outcomes = arm_control::application::runTasks(planner, tasks);
  return arm_control::adapters::reportOutcomes(session.logger(), outcomes);
}
}  // namespace

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  const int exit_code = runDemo();
  rclcpp::shutdown();
  return exit_code;
}
