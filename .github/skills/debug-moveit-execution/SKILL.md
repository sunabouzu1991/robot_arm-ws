---
name: debug-moveit-execution
description: Диагностика ошибок исполнения MoveIt 2 (CONTROL_FAILED -4, отклонение траектории, конфликт часов). Использовать при ошибках исполнения, когда планирование проходит успешно.
argument-hint: "[симптом] [команда воспроизведения]"
---

# Диагностика исполнения MoveIt 2

## Когда применять
- MoveIt error code `-4` (`CONTROL_FAILED`).
- `Execute request aborted` при успешном `Planning request complete`.
- Траектория строится, но контроллер её отклоняет.

## Порядок диагностики

### Шаг 1. Часы симуляции
Проверь `use_sim_time` во **всех трёх** местах:
1. `move_group` — в `gazebo_demo.launch.py`.
2. `rviz2` — там же.
3. Клиент `arm_control` — через `--ros-args -p use_sim_time:=true`.

См. [ADR-006](../../../docs/decisions/ADR-006-simulation-clock.md).

### Шаг 2. Связность контроллеров
- `FollowJointTrajectory` в `moveit_controllers.yaml`.
- `JointTrajectoryController` в `ros2_controllers.yaml`.
- Проверь имена, `action_ns`, `default: true`.

```bash
ros2 control list_controllers
```
### Шаг 3. Воспроизведение
```bash
ros2 launch arm_moveit_config gazebo_demo.launch.py
# в другом терминале
ros2 run arm_control move_tool_pose \
  --ros-args --params-file install/arm_control/share/arm_control/config/motion.yaml \
  -p use_sim_time:=true
```
### Шаг 4. Состояние контроллера
```bash
ros2 topic echo /arm_controller/state
ros2 topic echo /joint_states
```
## Что не делать
- Править сгенерированный `move_group.launch.py` — регенерируется Setup Assistant.
- Выставлять `use_sim_time` только в клиенте — метки траектории считает `move_group`.
- Поднимать mock и Gazebo одновременно — будет два `controller_manager` (ADR-004).

## Отчёт
- Симптом и команда воспроизведения.
- Найденная причина со ссылкой на ADR.
- Минимальное исправление.
- Команда проверки и ожидаемый результат.