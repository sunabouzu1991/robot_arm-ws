#include "arm_control/domain/joint_target.hpp"

#include <algorithm>
#include <cmath>

namespace arm_control::domain
{

std::optional<JointTarget> makeJointTarget(std::vector<double> positions)
{
  if (positions.empty()) {
    return std::nullopt;
  }

  const bool all_finite =
    std::all_of(positions.begin(), positions.end(), [](double value) { return std::isfinite(value); });
  if (!all_finite) {
    return std::nullopt;
  }

  return JointTarget{std::move(positions)};
}

bool matchesJointCount(const JointTarget& target, std::size_t joint_count)
{
  return target.positions.size() == joint_count;
}

}  // namespace arm_control::domain
