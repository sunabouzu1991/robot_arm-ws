#include "arm_control/application/motion_task_runner.hpp"

namespace arm_control::application
{

std::vector<domain::MotionOutcome> runTasks(ports::MotionPlanner& planner,
                                            const std::vector<domain::MotionTask>& tasks)
{
  std::vector<domain::MotionOutcome> outcomes;
  outcomes.reserve(tasks.size());

  for (const auto& task : tasks) {
    const domain::MotionOutcome outcome = planner.perform(task);
    const bool failed = !domain::succeeded(outcome);
    outcomes.push_back(outcome);
    if (failed) {
      break;
    }
  }

  return outcomes;
}

}  // namespace arm_control::application
