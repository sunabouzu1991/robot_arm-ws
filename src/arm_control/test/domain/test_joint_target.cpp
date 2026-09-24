// Контракт domain/joint_target: цель создаётся только из валидных значений,
// размерность сверяется с моделью робота, а не с константой в коде.

#include <limits>

#include <gtest/gtest.h>

#include "arm_control/domain/joint_target.hpp"

namespace
{
TEST(JointTarget, AcceptsFiniteValues)
{
  const auto target = arm_control::domain::makeJointTarget({0.1, -0.2, 0.3});

  ASSERT_TRUE(target.has_value());
  EXPECT_EQ(target->positions.size(), 3U);
}

TEST(JointTarget, RejectsEmptyTarget)
{
  EXPECT_FALSE(arm_control::domain::makeJointTarget({}).has_value());
}

TEST(JointTarget, RejectsNonFiniteValues)
{
  const double not_a_number = std::numeric_limits<double>::quiet_NaN();

  EXPECT_FALSE(arm_control::domain::makeJointTarget({0.1, not_a_number}).has_value());
  EXPECT_FALSE(
    arm_control::domain::makeJointTarget({std::numeric_limits<double>::infinity()}).has_value());
}

TEST(JointTarget, MatchesJointCountOfRobotModel)
{
  const auto target = arm_control::domain::makeJointTarget({0.0, 0.0, 0.0, 0.0, 0.0, 0.0});

  ASSERT_TRUE(target.has_value());
  EXPECT_TRUE(arm_control::domain::matchesJointCount(*target, 6U));
  EXPECT_FALSE(arm_control::domain::matchesJointCount(*target, 2U));
}

}  // namespace
