#pragma once

#include <string>

namespace arm_control::domain
{

/// Настройки сессии планирования: какие имена и коэффициенты использовать.
/// Значения приходят из параметров ROS, единственный источник — config/motion.yaml.
struct MotionSettings
{
  std::string planning_group;
  std::string base_frame;
  std::string tool_frame;
  std::string home_pose_name;
  double velocity_scaling = 0.5;
  double acceleration_scaling = 0.5;
};

/// Пустая строка, если настройки корректны; иначе описание первой проблемы.
std::string validationError(const MotionSettings& settings);

}  // namespace arm_control::domain
