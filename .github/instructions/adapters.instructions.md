---
description: 'Правила слоя adapters в arm_control — единственное место, знающее о MoveIt.'
applyTo: "arm_control/adapters/**"
---
# Слой adapters (arm_control)

## Границы
- Здесь и только здесь допустимы `moveit`, `rclcpp`, `geometry_msgs`.
- `MoveItMotionPlanner` — единственный адаптер, реализующий `ports::MotionPlanner`.
- `MoveItSession` — владеет `rclcpp::Node` и фоновым спиннером, создаётся и останавливается по RAII.

## Что можно
- `MoveGroupInterface` создаётся в конструкторе `MoveItMotionPlanner`, живёт вместе с адаптером.
- Команды планировщику и контроллерам — только через интерфейс MoveIt.

## Что нельзя
- Читать конфигурационные файлы напрямую — конфигурация приходит через `MotionSettings`.
- Обращаться к RViz, Gazebo, launch-параметрам.
- Хранить глобальное состояние между вызовами (кроме состояния, требуемого самим `MoveGroupInterface`).

## Threading
- `MoveItMotionPlanner` вызывается из главного потока.
- Ответы action-серверов приходят в спинере `MoveItSession`.
- Не блокируй главный поток операциями, которые должны идти через спиннер.