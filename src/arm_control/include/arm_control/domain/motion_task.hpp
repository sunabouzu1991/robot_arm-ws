#pragma once

#include <optional>
#include <string>
#include <variant>

#include "arm_control/domain/joint_target.hpp"
#include "arm_control/domain/tool_pose.hpp"

namespace arm_control::domain
{

/// Допуски попадания в цель, которые запрашивает команда.
struct Tolerances
{
  double position_m = 0.005;
  double orientation_rad = 0.01;
};

/// Цель: углы суставов в порядке модели робота.
struct JointTargetTask
{
  JointTarget target;
};

/// Цель: поза инструмента в base frame; решателю доступны все 6 DOF.
struct ToolPoseTask
{
  ToolPose target;
  Tolerances tolerances;
};

/// Цель: именованная конфигурация из SRDF (например, "home").
struct NamedPoseTask
{
  std::string pose_name;
};

/// Команда движения. Один вариант = один способ задать цель.
using MotionTask = std::variant<JointTargetTask, ToolPoseTask, NamedPoseTask>;

/// Описание команды для логов и сообщений об ошибке.
std::string describe(const MotionTask& task);

/// Допуски команды; nullopt, если команда их не задаёт.
std::optional<Tolerances> tolerancesOf(const MotionTask& task);

}  // namespace arm_control::domain
