#include "arm_control/domain/motion_settings.hpp"

#include <cmath>
#include <vector>

namespace arm_control::domain
{
namespace
{
constexpr double kScalingUpperBound = 1.0;

struct RequiredName
{
  const std::string* value;
  const char* parameter;
};

std::string describeScalingProblem(const char* parameter, double value)
{
  if (!std::isfinite(value) || value <= 0.0 || value > kScalingUpperBound) {
    return std::string("parameter '") + parameter + "' must be in (0, 1], got " + std::to_string(value);
  }
  return {};
}
}  // namespace

std::string validationError(const MotionSettings& settings)
{
  const std::vector<RequiredName> required_names = {
    {&settings.planning_group, "planning_group"},
    {&settings.base_frame, "base_frame"},
    {&settings.tool_frame, "tool_frame"},
    {&settings.home_pose_name, "home_pose_name"},
  };

  for (const auto& required : required_names) {
    if (required.value->empty()) {
      return std::string("required parameter '") + required.parameter +
             "' is not set; pass config/motion.yaml via the launch file or --ros-args --params-file";
    }
  }

  const std::string velocity_problem = describeScalingProblem("velocity_scaling", settings.velocity_scaling);
  if (!velocity_problem.empty()) {
    return velocity_problem;
  }

  return describeScalingProblem("acceleration_scaling", settings.acceleration_scaling);
}

}  // namespace arm_control::domain
