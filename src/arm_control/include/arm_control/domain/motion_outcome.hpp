#pragma once

#include <string>

namespace arm_control::domain
{

enum class MotionStatus
{
  kSucceeded,
  kRejectedByDomain,
  kPlanningFailed,
  kExecutionFailed,
  kSettingsInvalid,
};

/// Результат выполнения команды: статус плюс деталь для лога.
struct MotionOutcome
{
  MotionStatus status = MotionStatus::kPlanningFailed;
  std::string detail;
};

bool succeeded(const MotionOutcome& outcome);

std::string describe(MotionStatus status);

}  // namespace arm_control::domain
