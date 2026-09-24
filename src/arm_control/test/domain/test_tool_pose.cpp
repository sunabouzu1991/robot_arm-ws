// Контракт domain/tool_pose: инвариант нормировки, RPY-конвенция, рассогласование поз.

#include <cmath>

#include <gtest/gtest.h>

#include "arm_control/domain/tool_pose.hpp"

namespace
{
constexpr double kTolerance = 1e-9;

TEST(ToolPose, NormalizesOrientationOnConstruction)
{
  const auto pose = arm_control::domain::makeToolPose(
    arm_control::domain::Vector3{1.0, 2.0, 3.0}, arm_control::domain::Quaternion{0.0, 0.0, 0.0, 2.0});

  EXPECT_NEAR(pose.orientation.w, 1.0, kTolerance);
  EXPECT_NEAR(pose.orientation.x, 0.0, kTolerance);
}

TEST(ToolPose, ZeroQuaternionFallsBackToIdentity)
{
  const auto pose = arm_control::domain::makeToolPose(
    arm_control::domain::Vector3{}, arm_control::domain::Quaternion{0.0, 0.0, 0.0, 0.0});

  EXPECT_NEAR(pose.orientation.w, 1.0, kTolerance);
}

TEST(ToolPose, IdentityOrientationHasZeroRpy)
{
  const auto rpy = arm_control::domain::toRpy(arm_control::domain::Quaternion{0.0, 0.0, 0.0, 1.0});

  EXPECT_NEAR(rpy.roll, 0.0, kTolerance);
  EXPECT_NEAR(rpy.pitch, 0.0, kTolerance);
  EXPECT_NEAR(rpy.yaw, 0.0, kTolerance);
}

TEST(ToolPose, ReportsYawOfQuarterTurnAroundZ)
{
  const double half_angle = M_PI / 4.0;
  const auto rpy = arm_control::domain::toRpy(
    arm_control::domain::Quaternion{0.0, 0.0, std::sin(half_angle), std::cos(half_angle)});

  EXPECT_NEAR(rpy.yaw, M_PI / 2.0, 1e-9);
  EXPECT_NEAR(rpy.roll, 0.0, kTolerance);
}

TEST(ToolPose, DeviationMeasuresPositionAndRotation)
{
  const auto origin = arm_control::domain::makeToolPose(
    arm_control::domain::Vector3{0.0, 0.0, 0.0}, arm_control::domain::Quaternion{});
  const auto shifted = arm_control::domain::makeToolPose(
    arm_control::domain::Vector3{0.03, 0.04, 0.0}, arm_control::domain::Quaternion{});

  const auto deviation = arm_control::domain::deviation(origin, shifted);

  EXPECT_NEAR(deviation.position_m, 0.05, 1e-12);
  EXPECT_NEAR(deviation.orientation_rad, 0.0, kTolerance);
}

TEST(ToolPose, DeviationIsSignAgnosticForQuaternions)
{
  const auto reference = arm_control::domain::makeToolPose(
    arm_control::domain::Vector3{}, arm_control::domain::Quaternion{0.0, 0.0, 0.0, 1.0});
  const auto negated = arm_control::domain::makeToolPose(
    arm_control::domain::Vector3{}, arm_control::domain::Quaternion{0.0, 0.0, 0.0, -1.0});

  EXPECT_NEAR(arm_control::domain::deviation(reference, negated).orientation_rad, 0.0, kTolerance);
}

}  // namespace
