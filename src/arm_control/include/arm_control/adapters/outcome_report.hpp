#pragma once

#include <cstdlib>
#include <vector>

#include <rclcpp/rclcpp.hpp>

#include "arm_control/domain/motion_outcome.hpp"

namespace arm_control::adapters
{

/// Печатает результаты прогона и превращает их в код возврата процесса.
/// Отдельный адаптер, потому что логирование — инфраструктура: слой domain
/// не должен знать о rclcpp, а приложения не должны дублировать разбор статусов.
int reportOutcomes(const rclcpp::Logger& logger, const std::vector<domain::MotionOutcome>& outcomes);

}  // namespace arm_control::adapters
