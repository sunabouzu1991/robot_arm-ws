// Точка входа демонстрации: движение по углам суставов и возврат в home.
// Здесь только композиция слоёв и демонстрационные данные — логики планирования нет.

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
const std::string kNodeName = "move_joints";

// Углы суставов (рад) в порядке модели робота: пояс, плечо, локоть, крен/тангаж/поворот кисти.
// Конфигурация проверена на отсутствие самоколлизий и на попадание в лимиты URDF.
const std::vector<double> kWorkJointPositions = {0.3, 0.2, -0.3, 0.1, 0.2, 0.1};

std::vector<arm_control::domain::MotionTask> buildTasks(const arm_control::domain::MotionSettings& settings)
{
  const auto work_target = arm_control::domain::makeJointTarget(kWorkJointPositions);
  if (!work_target) {
    return {};
  }

  return {
    arm_control::domain::JointTargetTask{*work_target},
    arm_control::domain::NamedPoseTask{settings.home_pose_name},
  };
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
  RCLCPP_INFO(
    session.logger(), "Planning group '%s' exposes %zu joints", settings.planning_group.c_str(),
    planner.jointNames().size());

  const auto outcomes = arm_control::application::runTasks(planner, buildTasks(settings));
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
