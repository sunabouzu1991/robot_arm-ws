---
description: 'Правила слоя domain в arm_control — чистая логика без ROS.'
applyTo: "arm_control/domain/**"
---
# Слой domain (arm_control)

## Границы
- Допустимые зависимости: только STL (`<optional>`, `<variant>`, `<vector>`, `<string>`, `<chrono>`).
- Запрещено: `rclcpp`, `moveit`, `geometry_msgs`, `rclcpp_action`, любые заголовки ROS/MoveIt.
- Нарушение обнаруживается на этапе линковки: цель `arm_control_domain` не вызывает `ament_target_dependencies`.

## Ответственность
- Типы и инварианты: позы, цели (`JointTargetTask`, `ToolPoseTask`, `NamedPoseTask`), допуски, статусы, настройки.
- Валидация: `domain::validationError` для настроек движения (поле обязательно, коэффициент в `(0, 1]`).
- Логика проверок живёт здесь, а не в адаптере (см. ADR-003).

## Стиль
- Value semantics: типы передаются копией, а не по указателю.
- Публичные типы — `struct`/`class` с явными конструкторами, без сеттеров.
- Никаких синглтонов, глобального состояния и статических мутабельных переменных.
- Тесты — gtest + STL, без запуска ROS-графа (см. `arm_control/test/`).