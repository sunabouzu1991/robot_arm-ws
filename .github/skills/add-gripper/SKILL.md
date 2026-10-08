---
name: add-gripper
description: Добавляет схват или гриппер к манипулятору — отдельный xacro-макрос, физическая привязка к tcp, отдельная SRDF-группа. Использовать при расширении робота рабочим органом.
argument-hint: "[имя-схвата]"
---

# Добавление схвата

## Когда применять
- Пользователь просит добавить рабочий орган (схват, гриппер, вакуумную присоску) к манипулятору.

## Шаги

### 1. Xacro-макрос с инерцией
Создать отдельный макрос, не встраивать в `arm.urdf.xacro`.

- Файл: `arm_description/urdf/<имя_схвата>.urdf.xacro`.
- Макрос: `<xacro:macro name="<имя_схвата>" params="prefix parent">`.
- Обязательно: масса, инерция (тензор), визуал, коллизия.

### 2. Физическая привязка к tcp
- Родительское звено — `tcp` (фланец).
- Joint типа `fixed` или `revolute`/`prismatic` для подвижных частей.
- Проверить, что `<origin>` корректен относительно `tcp`.

### 3. SRDF — отдельная группа
Файл: `arm_moveit_config/config/arm.srdf`.

```xml
<group name="<имя_схвата>">
  <chain base_link="tcp" tip_link="<имя_схвата>_tip"/>
</group>
```
- Не добавлять суставы схвата в `arm` или `wrist`.
- Если схват имеет несколько пальцев — создать подгруппы или отдельные группы на палец.

### 4. Контроллер (если схват активный)
- `ros2_controllers.yaml`: новый `JointTrajectoryController` или `GripperActionController`.
- `moveit_controllers.yaml`: соответствующий `FollowJointTrajectory`.
- Имена и action_ns должны совпадать буквально.

### 5. Лимиты
`joint_limits.yaml`:
``` yaml
<имя_схвата>_joint:
  has_velocity_limits: true
  max_velocity: <значение>
  has_acceleration_limits: true
  max_acceleration: <значение>
```
### 6. Запуск
- В `display.launch.py` — добавить параметр для подключения схвата.
- Проверить в RViz: `<robot_description>` содержит новые звенья.

## Что нельзя
- Встраивать геометрию схвата прямо в `arm.urdf.xacro`.
- Добавлять суставы схвата в группу `arm` или `wrist`.
- Забывать инерционные параметры — `Gazebo` будет вести себя непредсказуемо.
- Использовать `<depend>` для `gazebo_ros2_control` в `package.xml` — только `<exec_depend>`.

## Проверка
```bash
colcon build --symlink-install
ros2 launch arm_description display.launch.py
# в RViz: проверить наличие <имя_схвата> в дереве
ros2 launch arm_moveit_config demo.launch.py
# в RViz: в MotionPlanning выбрать группу <имя_схвата>
```