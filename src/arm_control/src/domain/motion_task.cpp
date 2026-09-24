#include "arm_control/domain/motion_task.hpp"

#include <sstream>
#include <type_traits>

namespace arm_control::domain
{
namespace
{
std::string describeJointTarget(const JointTargetTask& task)
{
  std::ostringstream stream;
  stream << "joint target [";
  for (std::size_t index = 0; index < task.target.positions.size(); ++index) {
    if (index != 0U) {
      stream << ", ";
    }
    stream << task.target.positions[index];
  }
  stream << "] rad";
  return stream.str();
}

std::string describeToolPose(const ToolPoseTask& task)
{
  std::ostringstream stream;
  stream << "tool pose [X: " << task.target.position.x << ", Y: " << task.target.position.y
         << ", Z: " << task.target.position.z << " | RPY: ";
  const Rpy rpy = toRpy(task.target.orientation);
  stream << rpy.roll << ", " << rpy.pitch << ", " << rpy.yaw << "]";
  return stream.str();
}
}  // namespace

std::string describe(const MotionTask& task)
{
  return std::visit(
    [](const auto& concrete) -> std::string {
      using TaskType = std::decay_t<decltype(concrete)>;
      if constexpr (std::is_same_v<TaskType, JointTargetTask>) {
        return describeJointTarget(concrete);
      } else if constexpr (std::is_same_v<TaskType, ToolPoseTask>) {
        return describeToolPose(concrete);
      } else {
        return "named pose '" + concrete.pose_name + "'";
      }
    },
    task);
}

std::optional<Tolerances> tolerancesOf(const MotionTask& task)
{
  const auto* tool_pose_task = std::get_if<ToolPoseTask>(&task);
  if (tool_pose_task == nullptr) {
    return std::nullopt;
  }
  return tool_pose_task->tolerances;
}

}  // namespace arm_control::domain
