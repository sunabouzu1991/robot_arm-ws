#pragma once

#include <string>
#include <vector>

#include <moveit/move_group_interface/move_group_interface.h>
#include <rclcpp/rclcpp.hpp>

#include "arm_control/domain/motion_settings.hpp"
#include "arm_control/ports/motion_planner.hpp"

namespace arm_control::adapters
{

/// Адаптер порта ports::MotionPlanner поверх MoveIt MoveGroupInterface.
/// Всё знание о MoveIt сосредоточено здесь: домен и приложения видят только порт,
/// поэтому смену планировщика (OMPL / PILZ / свой) не нужно искать по всему коду.
class MoveItMotionPlanner final : public ports::MotionPlanner
{
public:
  MoveItMotionPlanner(const rclcpp::Node::SharedPtr& node, const domain::MotionSettings& settings);

  std::vector<std::string> jointNames() const override;
  std::optional<domain::ToolPose> currentToolPose() override;
  domain::MotionOutcome perform(const domain::MotionTask& task) override;

private:
  domain::MotionOutcome performJointTarget(const domain::JointTargetTask& task);
  domain::MotionOutcome performToolPose(const domain::ToolPoseTask& task);
  domain::MotionOutcome performNamedPose(const domain::NamedPoseTask& task);

  /// Планирование и исполнение уже выставленной цели.
  domain::MotionOutcome planAndExecute(const domain::MotionTask& task);

  rclcpp::Node::SharedPtr node_;
  domain::MotionSettings settings_;
  moveit::planning_interface::MoveGroupInterface move_group_;
};

}  // namespace arm_control::adapters
