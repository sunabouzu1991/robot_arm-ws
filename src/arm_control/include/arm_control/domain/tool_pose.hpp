#pragma once

// Слой domain: чистая кинематика инструмента.
// Зависимостей от ROS и MoveIt нет — цель сборки arm_control_domain их не видит.

namespace arm_control::domain
{

struct Vector3
{
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
};

struct Quaternion
{
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  double w = 1.0;
};

/// Углы Эйлера в порядке roll-pitch-yaw (рад) — та же конвенция, что в URDF.
struct Rpy
{
  double roll = 0.0;
  double pitch = 0.0;
  double yaw = 0.0;
};

/// Поза инструмента в системе координат base frame робота.
struct ToolPose
{
  Vector3 position;
  Quaternion orientation;
};

/// Инвариант домена: ориентация ToolPose всегда нормирована.
ToolPose makeToolPose(Vector3 position, Quaternion orientation);

/// Кватернион единичной длины; при нулевой норме — тождественный (0, 0, 0, 1).
Quaternion normalized(Quaternion q);

/// Рассогласование двух поз: метры по позиции и радианы по углу поворота.
struct PoseDeviation
{
  double position_m = 0.0;
  double orientation_rad = 0.0;
};

PoseDeviation deviation(const ToolPose& from, const ToolPose& to);

Rpy toRpy(const Quaternion& q);

}  // namespace arm_control::domain
