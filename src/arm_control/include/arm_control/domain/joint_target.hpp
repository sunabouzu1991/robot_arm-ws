#pragma once

#include <cstddef>
#include <optional>
#include <vector>

namespace arm_control::domain
{

/// Целевые углы суставов (рад) в порядке, который задаёт модель робота.
/// Список имён суставов здесь не дублируется: он берётся у планировщика
/// (ports::MotionPlanner::jointNames), то есть из URDF.
struct JointTarget
{
  std::vector<double> positions;
};

/// Создаёт цель, если значения заданы и все конечны; иначе std::nullopt.
std::optional<JointTarget> makeJointTarget(std::vector<double> positions);

/// Проверяет соответствие цели размерности модели робота.
bool matchesJointCount(const JointTarget& target, std::size_t joint_count);

}  // namespace arm_control::domain
