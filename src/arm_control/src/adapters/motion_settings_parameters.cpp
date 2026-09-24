#include "arm_control/adapters/motion_settings_parameters.hpp"

namespace arm_control::adapters
{

domain::MotionSettings readMotionSettings(rclcpp::Node& node)
{
  domain::MotionSettings settings;
  settings.planning_group = parameterOr<std::string>(node, "planning_group", "");
  settings.base_frame = parameterOr<std::string>(node, "base_frame", "");
  settings.tool_frame = parameterOr<std::string>(node, "tool_frame", "");
  settings.home_pose_name = parameterOr<std::string>(node, "home_pose_name", "");
  settings.velocity_scaling = parameterOr<double>(node, "velocity_scaling", 0.5);
  settings.acceleration_scaling = parameterOr<double>(node, "acceleration_scaling", 0.5);
  return settings;
}

}  // namespace arm_control::adapters
