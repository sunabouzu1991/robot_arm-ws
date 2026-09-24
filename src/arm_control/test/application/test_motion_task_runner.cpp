// Контракт application/motion_task_runner: порядок выполнения и остановка
// на первой неудаче. Подставной планировщик доказывает, что порт действительно
// отделён от MoveIt: тест не линкуется ни с MoveIt, ни с rclcpp.

#include <cstddef>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "arm_control/application/motion_task_runner.hpp"

namespace
{
using arm_control::domain::MotionOutcome;
using arm_control::domain::MotionStatus;
using arm_control::domain::MotionTask;
using arm_control::domain::NamedPoseTask;
using arm_control::domain::ToolPoseTask;

class ScriptedPlanner : public arm_control::ports::MotionPlanner
{
public:
  explicit ScriptedPlanner(std::vector<MotionStatus> script) : script_{std::move(script)} {}

  std::vector<std::string> jointNames() const override { return {"joint_1", "joint_2"}; }

  std::optional<arm_control::domain::ToolPose> currentToolPose() override { return std::nullopt; }

  MotionOutcome perform(const arm_control::domain::MotionTask& task) override
  {
    requested.push_back(arm_control::domain::describe(task));
    const MotionStatus status =
      index_ < script_.size() ? script_[index_] : MotionStatus::kSucceeded;
    ++index_;
    return MotionOutcome{status, arm_control::domain::describe(task)};
  }

  std::vector<std::string> requested;

private:
  std::vector<MotionStatus> script_;
  std::size_t index_ = 0;
};

std::vector<MotionTask> threeTasks()
{
  return {
    ToolPoseTask{arm_control::domain::makeToolPose({}, {}), arm_control::domain::Tolerances{}},
    NamedPoseTask{"middle"},
    NamedPoseTask{"home"},
  };
}
}  // namespace

TEST(MotionTaskRunner, RunsEveryTaskInOrder)
{
  ScriptedPlanner planner{{MotionStatus::kSucceeded, MotionStatus::kSucceeded,
                           MotionStatus::kSucceeded}};

  const auto outcomes = arm_control::application::runTasks(planner, threeTasks());

  ASSERT_EQ(outcomes.size(), 3U);
  EXPECT_EQ(planner.requested.size(), 3U);
  EXPECT_EQ(planner.requested[1], "named pose 'middle'");
}

TEST(MotionTaskRunner, StopsAfterFirstFailure)
{
  ScriptedPlanner planner{{MotionStatus::kSucceeded, MotionStatus::kPlanningFailed,
                           MotionStatus::kSucceeded}};

  const auto outcomes = arm_control::application::runTasks(planner, threeTasks());

  ASSERT_EQ(outcomes.size(), 2U);
  EXPECT_EQ(outcomes.back().status, MotionStatus::kPlanningFailed);
  EXPECT_EQ(planner.requested.size(), 2U);
}

TEST(MotionTaskRunner, ReturnsEmptyOutcomeListForEmptyTaskList)
{
  ScriptedPlanner planner{{}};

  EXPECT_TRUE(arm_control::application::runTasks(planner, {}).empty());
}
