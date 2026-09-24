// Контракт domain/motion_task и domain/motion_settings: команды описываются
// одинаково во всех логах, допуски доступны только там, где они есть,
// а неполные настройки дают понятную ошибку вместо тихого дефолта.

#include <gtest/gtest.h>

#include "arm_control/domain/motion_settings.hpp"
#include "arm_control/domain/motion_outcome.hpp"
#include "arm_control/domain/motion_task.hpp"

namespace
{
using arm_control::domain::JointTargetTask;
using arm_control::domain::MotionSettings;
using arm_control::domain::MotionTask;
using arm_control::domain::NamedPoseTask;
using arm_control::domain::ToolPoseTask;

MotionSettings validSettings()
{
  MotionSettings settings;
  settings.planning_group = "arm";
  settings.base_frame = "base_link";
  settings.tool_frame = "tcp";
  settings.home_pose_name = "home";
  return settings;
}

TEST(MotionTask, DescribesJointTargetInRadians)
{
  const auto target = arm_control::domain::makeJointTarget({0.5, -1.0});
  ASSERT_TRUE(target.has_value());

  const MotionTask task = JointTargetTask{*target};

  EXPECT_EQ(arm_control::domain::describe(task), "joint target [0.5, -1] rad");
}

TEST(MotionTask, DescribesNamedPose)
{
  const MotionTask task = NamedPoseTask{"home"};

  EXPECT_EQ(arm_control::domain::describe(task), "named pose 'home'");
}

TEST(MotionTask, ExposesTolerancesOnlyForToolPose)
{
  const ToolPoseTask tool_pose_task{
    arm_control::domain::makeToolPose({}, {}), arm_control::domain::Tolerances{0.001, 0.002}};
  const MotionTask tool_pose = tool_pose_task;

  ASSERT_TRUE(arm_control::domain::tolerancesOf(tool_pose).has_value());
  EXPECT_DOUBLE_EQ(arm_control::domain::tolerancesOf(tool_pose)->position_m, 0.001);

  const auto joint_target = arm_control::domain::makeJointTarget({0.0});
  ASSERT_TRUE(joint_target.has_value());
  const MotionTask joint_task = JointTargetTask{*joint_target};

  EXPECT_FALSE(arm_control::domain::tolerancesOf(joint_task).has_value());
}

TEST(MotionSettings, AcceptsCompleteSettings)
{
  EXPECT_TRUE(arm_control::domain::validationError(validSettings()).empty());
}

TEST(MotionSettings, NamesMissingParameter)
{
  MotionSettings settings = validSettings();
  settings.tool_frame.clear();

  EXPECT_NE(arm_control::domain::validationError(settings).find("tool_frame"), std::string::npos);
}

TEST(MotionSettings, RejectsScalingOutOfRange)
{
  MotionSettings settings = validSettings();
  settings.velocity_scaling = 1.5;

  EXPECT_NE(
    arm_control::domain::validationError(settings).find("velocity_scaling"), std::string::npos);
}

TEST(MotionOutcome, OnlySuccessCountsAsSucceeded)
{
  EXPECT_TRUE(arm_control::domain::succeeded({arm_control::domain::MotionStatus::kSucceeded, ""}));
  EXPECT_FALSE(
    arm_control::domain::succeeded({arm_control::domain::MotionStatus::kPlanningFailed, ""}));
}

}  // namespace
