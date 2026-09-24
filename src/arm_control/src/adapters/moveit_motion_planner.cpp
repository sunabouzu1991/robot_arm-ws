#include "arm_control/adapters/moveit_motion_planner.hpp"

#include <type_traits>
#include <utility>

#include <geometry_msgs/msg/pose.hpp>

namespace arm_control::adapters
{
namespace
{
geometry_msgs::msg::Pose toMessage(const domain::ToolPose& pose)
{
  geometry_msgs::msg::Pose message;
  message.position.x = pose.position.x;
  message.position.y = pose.position.y;
  message.position.z = pose.position.z;
  message.orientation.x = pose.orientation.x;
  message.orientation.y = pose.orientation.y;
  message.orientation.z = pose.orientation.z;
  message.orientation.w = pose.orientation.w;
  return message;
}

domain::ToolPose toDomain(const geometry_msgs::msg::Pose& pose)
{
  return domain::makeToolPose(
    domain::Vector3{pose.position.x, pose.position.y, pose.position.z},
    domain::Quaternion{pose.orientation.x, pose.orientation.y, pose.orientation.z, pose.orientation.w});
}

std::string describeMoveItError(const moveit::core::MoveItErrorCode& code)
{
  return "MoveIt error code " + std::to_string(code.val);
}
}  // namespace

MoveItMotionPlanner::MoveItMotionPlanner(const rclcpp::Node::SharedPtr& node,
                                         const domain::MotionSettings& settings)
: node_{node}, settings_{settings}, move_group_{node, settings.planning_group}
{
  move_group_.setMaxVelocityScalingFactor(settings_.velocity_scaling);
  move_group_.setMaxAccelerationScalingFactor(settings_.acceleration_scaling);
  move_group_.setPoseReferenceFrame(settings_.base_frame);
  move_group_.setEndEffectorLink(settings_.tool_frame);
}

std::vector<std::string> MoveItMotionPlanner::jointNames() const
{
  return move_group_.getJointNames();
}

std::optional<domain::ToolPose> MoveItMotionPlanner::currentToolPose()
{
  const auto pose = move_group_.getCurrentPose(settings_.tool_frame);
  if (pose.header.frame_id.empty()) {
    return std::nullopt;
  }
  return toDomain(pose.pose);
}

domain::MotionOutcome MoveItMotionPlanner::perform(const domain::MotionTask& task)
{
  return std::visit(
    [this](const auto& concrete) -> domain::MotionOutcome {
      using TaskType = std::decay_t<decltype(concrete)>;
      if constexpr (std::is_same_v<TaskType, domain::JointTargetTask>) {
        return performJointTarget(concrete);
      } else if constexpr (std::is_same_v<TaskType, domain::ToolPoseTask>) {
        return performToolPose(concrete);
      } else {
        return performNamedPose(concrete);
      }
    },
    task);
}

domain::MotionOutcome MoveItMotionPlanner::performJointTarget(const domain::JointTargetTask& task)
{
  const std::size_t model_joint_count = move_group_.getJointNames().size();
  if (!domain::matchesJointCount(task.target, model_joint_count)) {
    return {domain::MotionStatus::kRejectedByDomain,
            domain::describe(task) + " -> target has " + std::to_string(task.target.positions.size()) +
              " joints, robot model has " + std::to_string(model_joint_count)};
  }

  if (!move_group_.setJointValueTarget(task.target.positions)) {
    return {domain::MotionStatus::kRejectedByDomain,
            domain::describe(task) + " -> values exceed joint limits declared in the URDF"};
  }

  return planAndExecute(task);
}

domain::MotionOutcome MoveItMotionPlanner::performToolPose(const domain::ToolPoseTask& task)
{
  move_group_.setGoalPositionTolerance(task.tolerances.position_m);
  move_group_.setGoalOrientationTolerance(task.tolerances.orientation_rad);
  move_group_.setPoseTarget(toMessage(task.target), settings_.tool_frame);
  return planAndExecute(task);
}

domain::MotionOutcome MoveItMotionPlanner::performNamedPose(const domain::NamedPoseTask& task)
{
  move_group_.setNamedTarget(task.pose_name);
  return planAndExecute(task);
}

domain::MotionOutcome MoveItMotionPlanner::planAndExecute(const domain::MotionTask& task)
{
  moveit::planning_interface::MoveGroupInterface::Plan plan;
  const auto plan_code = move_group_.plan(plan);
  if (plan_code != moveit::core::MoveItErrorCode::SUCCESS) {
    return {domain::MotionStatus::kPlanningFailed,
            domain::describe(task) + " -> " + describeMoveItError(plan_code)};
  }

  const auto execute_code = move_group_.execute(plan);
  if (execute_code != moveit::core::MoveItErrorCode::SUCCESS) {
    return {domain::MotionStatus::kExecutionFailed,
            domain::describe(task) + " -> " + describeMoveItError(execute_code)};
  }

  return {domain::MotionStatus::kSucceeded, domain::describe(task)};
}

}  // namespace arm_control::adapters
