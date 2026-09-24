#include "arm_control/domain/tool_pose.hpp"

#include <algorithm>
#include <cmath>

namespace arm_control::domain
{
namespace
{
constexpr double kIdentityQuaternionEpsilon = 1e-12;
}  // namespace

Quaternion normalized(Quaternion q)
{
  const double norm = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
  if (norm < kIdentityQuaternionEpsilon) {
    return Quaternion{};
  }
  return Quaternion{q.x / norm, q.y / norm, q.z / norm, q.w / norm};
}

ToolPose makeToolPose(Vector3 position, Quaternion orientation)
{
  return ToolPose{position, normalized(orientation)};
}

PoseDeviation deviation(const ToolPose& from, const ToolPose& to)
{
  const double dx = to.position.x - from.position.x;
  const double dy = to.position.y - from.position.y;
  const double dz = to.position.z - from.position.z;

  const double dot = from.orientation.x * to.orientation.x +
                     from.orientation.y * to.orientation.y +
                     from.orientation.z * to.orientation.z +
                     from.orientation.w * to.orientation.w;

  const double angle = 2.0 * std::acos(std::clamp(std::abs(dot), 0.0, 1.0));

  return PoseDeviation{std::sqrt(dx * dx + dy * dy + dz * dz), angle};
}

Rpy toRpy(const Quaternion& q)
{
  const Quaternion unit = normalized(q);

  const double sinr_cosp = 2.0 * (unit.w * unit.x + unit.y * unit.z);
  const double cosr_cosp = 1.0 - 2.0 * (unit.x * unit.x + unit.y * unit.y);
  const double sinp = 2.0 * (unit.w * unit.y - unit.z * unit.x);
  const double siny_cosp = 2.0 * (unit.w * unit.z + unit.x * unit.y);
  const double cosy_cosp = 1.0 - 2.0 * (unit.y * unit.y + unit.z * unit.z);

  Rpy rpy;
  rpy.roll = std::atan2(sinr_cosp, cosr_cosp);
  rpy.pitch = std::abs(sinp) >= 1.0 ? std::copysign(M_PI / 2.0, sinp) : std::asin(sinp);
  rpy.yaw = std::atan2(siny_cosp, cosy_cosp);
  return rpy;
}

}  // namespace arm_control::domain
