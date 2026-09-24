#pragma once

#include <optional>
#include <string>
#include <vector>

#include "arm_control/domain/motion_outcome.hpp"
#include "arm_control/domain/motion_task.hpp"
#include "arm_control/domain/tool_pose.hpp"

namespace arm_control::ports
{

/// Контракт исполнителя движения. Реализация — adapters::MoveItMotionPlanner.
/// Порт не знает ни про MoveIt, ни про rclcpp: зависимость направлена
/// adapter -> port -> domain, обратной быть не может.
class MotionPlanner
{
public:
  virtual ~MotionPlanner() = default;

  /// Имена суставов планируемой группы в порядке модели робота (из URDF).
  virtual std::vector<std::string> jointNames() const = 0;

  /// Текущая поза инструмента. Метод не const: чтение блокируется до прихода
  /// свежего /joint_states, то есть меняет наблюдаемое состояние запроса.
  virtual std::optional<domain::ToolPose> currentToolPose() = 0;

  /// Выполняет команду целиком: планирование и исполнение траектории.
  virtual domain::MotionOutcome perform(const domain::MotionTask& task) = 0;
};

}  // namespace arm_control::ports
