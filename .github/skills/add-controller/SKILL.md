---
name: add-controller
description: Добавляет новый ros2_control контроллер с согласованными moveit_controllers.yaml и ros2_controllers.yaml. Использовать при добавлении контроллера.
argument-hint: "[имя-контроллера] [тип]"
---

# Добавление ros2_control контроллера

## Когда применять
- Нужен новый контроллер: суставной, позиционный, gripper, кастомный.

## Обязательная связность (ADR-002 и ADR-004)

Имена и интерфейсы в двух файлах должны совпадать **буквально**:
- `arm_moveit_config/config/moveit_controllers.yaml` — со стороны MoveIt.
- `arm_moveit_config/config/ros2_controllers.yaml` — со стороны `controller_manager`.

## Шаги

### 1. Определить тип
- `joint_trajectory_controller/JointTrajectoryController` — траектории в суставном пространстве.
- `position_controllers/JointGroupPositionController` — прямые позиции.
- `gripper_controllers/GripperActionController` — схват.

### 2. Запись в ros2_controllers.yaml

```yaml
<controller_name>:
  ros__parameters:
    joints:
      - <joint_1>
      - <joint_2>
    interface_name: position
    command_interfaces:
      - position
    state_interfaces:
      - position
      - velocity
```
### 3. Запись в moveit_controllers.yaml
```yaml
moveit_simple_controller_manager:
  controller_names:
    - <controller_name>

  <controller_name>:
    type: FollowJointTrajectory
    action_ns: follow_joint_trajectory
    default: true
    joints:
      - <joint_1>
      - <joint_2>
```
### 4. Проверка совпадения
- `action_ns` — точно как в `JointTrajectoryController`.
- `joints` — одинаковый порядок и имена.
- `default: true` — только для одного контроллера на группу.

### 5. Лимиты
- `joint_limits.yaml` — добавить записи для суставов, если их нет.

### 6. Запуск
```bash
ros2 launch arm_moveit_config demo.launch.py
ros2 control list_controllers
ros2 control list_hardware_interfaces
```
- Ожидаемый результат: контроллер в состоянии `active`.

## Что нельзя
- Писать `<controller_name>` по-разному в двух файлах — это самая частая причина «контроллер не стартует».
- Менять `default: true` без явной причины.
- Пропускать `state_interfaces` — MoveIt не увидит состояние.
- Объявлять контроллер только в одном из двух файлов.

## Отчёт
- Тип контроллера и обоснование.
- Diff обоих YAML.
- Команда проверки и ожидаемый `list_controllers`.