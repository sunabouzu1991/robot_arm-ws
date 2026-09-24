#pragma once

#include <vector>

#include "arm_control/ports/motion_planner.hpp"

namespace arm_control::application
{

/// Прогоняет команды строго по порядку и останавливается на первой неудаче.
/// Единственная ответственность слоя: политика порядка выполнения
/// и агрегация результатов. Планирование и логика робота — не здесь.
std::vector<domain::MotionOutcome> runTasks(ports::MotionPlanner& planner,
                                            const std::vector<domain::MotionTask>& tasks);

}  // namespace arm_control::application
