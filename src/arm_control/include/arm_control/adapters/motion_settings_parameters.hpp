#pragma once

#include <string>

#include <rclcpp/rclcpp.hpp>

#include "arm_control/domain/motion_settings.hpp"

namespace arm_control::adapters
{

/// Читает настройки сессии планирования из параметров узла.
/// Значения приходят из config/motion.yaml — в коде их дублей нет,
/// отсутствие обязательного параметра даёт понятную ошибку валидации.
domain::MotionSettings readMotionSettings(rclcpp::Node& node);

/// Возвращает параметр, объявляя его только если он не пришёл извне.
///
/// Узел приложения создаётся с automatically_declare_parameters_from_overrides,
/// поэтому значения из --params-file уже объявлены, а повторный declare_parameter
/// бросает ParameterAlreadyDeclaredException. Приоритет всегда у значения из файла;
/// fallback применяется только когда параметра нет вовсе.
template<typename T>
T parameterOr(rclcpp::Node& node, const std::string& name, const T& fallback)
{
  if (node.has_parameter(name)) {
    return node.get_parameter(name).get_value<T>();
  }
  return node.declare_parameter<T>(name, fallback);
}

}  // namespace arm_control::adapters
