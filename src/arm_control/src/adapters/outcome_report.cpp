#include "arm_control/adapters/outcome_report.hpp"

namespace arm_control::adapters
{

int reportOutcomes(const rclcpp::Logger& logger, const std::vector<domain::MotionOutcome>& outcomes)
{
  if (outcomes.empty()) {
    RCLCPP_ERROR(logger, "Nothing to do: the task list is empty");
    return EXIT_FAILURE;
  }

  for (const auto& outcome : outcomes) {
    if (domain::succeeded(outcome)) {
      RCLCPP_INFO(logger, "Done: %s", outcome.detail.c_str());
      continue;
    }
    RCLCPP_ERROR(logger, "%s: %s", domain::describe(outcome.status).c_str(), outcome.detail.c_str());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

}  // namespace arm_control::adapters
