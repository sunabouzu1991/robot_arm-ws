#include "arm_control/domain/motion_outcome.hpp"

namespace arm_control::domain
{

bool succeeded(const MotionOutcome& outcome)
{
  return outcome.status == MotionStatus::kSucceeded;
}

std::string describe(MotionStatus status)
{
  switch (status) {
    case MotionStatus::kSucceeded:
      return "succeeded";
    case MotionStatus::kRejectedByDomain:
      return "rejected by domain rules";
    case MotionStatus::kPlanningFailed:
      return "planning failed";
    case MotionStatus::kExecutionFailed:
      return "execution failed";
    case MotionStatus::kSettingsInvalid:
      return "invalid motion settings";
  }
  return "unknown status";
}

}  // namespace arm_control::domain
