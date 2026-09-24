# Архитектура

Документация описывает контракты, а не реализацию: её должно быть достаточно,
чтобы изменить одну функцию, не читая полворкспейса.

## Карта воркспейса

```text
arm_description   модель робота, миры, RViz-конфиги, launch-файлы визуализации и Gazebo
arm_moveit_config конфигурация MoveIt 2: SRDF, kinematics, joint_limits, контроллеры
arm_control       движение робота: команды и исполнение через MoveIt 2
```

## Направление зависимостей

```text
arm_description ─────► внешние пакеты (xacro, ros2_control), больше ни от чего
arm_moveit_config ───► arm_description (по именам робота и группы)
arm_control ─────────► arm_moveit_config (по именам группы, фреймов, поз)
   │
   ├─ apps        композиция слоёв + демо-данные
   ├─ adapters    MoveIt 2, rclcpp, geometry_msgs
   ├─ application политика выполнения команд
   ├─ ports       контракт исполнителя движения
   └─ domain      чистая логика, только STL
```

Правило: стрелки только вниз по списку. Обратная зависимость (например,
`domain → adapters`) невозможна технически: цель `arm_control_domain` не линкуется
ни с rclcpp, ни с MoveIt, поэтому такой `#include` просто не соберётся.
См. [ADR-002](../decisions/ADR-002-layered-arm-control.md).

## Компоненты

Ниже — по одному блоку на компонент. Формат одинаковый намеренно: это
машиночитаемый архитектурный API.

### MotionTask (domain)

- Responsibility: описать, чего мы хотим от робота, не зная как это достигается.
- Owns: варианты цели — `JointTargetTask`, `ToolPoseTask`, `NamedPoseTask`.
- Reads: ничего.
- Writes: ничего.
- Does not access: MoveIt, rclcpp, параметры ROS.
- Called by: `apps/*`, `MoveItMotionPlanner::perform`.
- Threading: значение, передаётся копией.
- Authority: клиент.

### MotionPlanner (ports)

- Responsibility: контракт исполнителя движения.
- Owns: сигнатуры `jointNames`, `currentToolPose`, `perform`.
- Reads: `MotionTask`, `ToolPose`.
- Writes: `MotionOutcome`.
- Does not access: MoveIt, rclcpp, глобальное состояние.
- Called by: `application::runTasks`.
- Threading: синхронный вызов, блокирует до конца планирования и исполнения.
- Authority: один экземпляр на процесс.

### MoveItMotionPlanner (adapters)

- Responsibility: единственное место, знающее о MoveIt 2.
- Owns: `MoveGroupInterface` (создаётся в конструкторе, живёт вместе с адаптером).
- Reads: `MotionSettings`, `MotionTask`.
- Writes: `MotionOutcome`, команды планировщику и контроллерам.
- Does not access: RViz, Gazebo, конфигурационные файлы напрямую.
- Called by: `apps/*` (через `application::runTasks`).
- Threading: вызывается из главного потока; ответы action-серверов приходят
  в спинере `MoveItSession`.
- Authority: клиент move_group.

### MoveItSession (adapters)

- Responsibility: владеть узлом rclcpp и фоновым спиннером.
- Owns: `rclcpp::Node`, `SingleThreadedExecutor`, поток спиннера.
- Reads: аргументы командной строки ROS (`--ros-args`).
- Writes: параметры узла.
- Does not access: MoveIt (кроме имени узла), код движения.
- Called by: `main` приложений; время жизни — весь прогон.
- Threading: один поток; создаётся и останавливается по RAII.
- Authority: локально для процесса.

### MotionSettings (domain + adapters/motion_settings_parameters)

- Responsibility: держать имена группы, фреймов, позы возврата и коэффициенты скорости.
- Owns: значения параметров; источник истины — `config/motion.yaml`.
- Reads: параметры ROS (`readMotionSettings`).
- Writes: ничего.
- Does not access: MoveIt, URDF.
- Called by: `apps/*` в начале прогона.
- Threading: читается один раз до старта движения.
- Authority: файл `config/motion.yaml`, см. [ADR-003](../decisions/ADR-003-motion-settings.md).

### ros2_control (arm_description + arm_moveit_config)

- Responsibility: исполнять траектории в суставном пространстве.
- Owns: хардварные интерфейсы (Gazebo plugin или mock_components).
- Reads: `FollowJointTrajectory` от `arm_controller`.
- Writes: `/joint_states` через `joint_state_broadcaster`.
- Does not access: планирование, IK.
- Called by: контроллеры `arm_controller`, `joint_state_broadcaster`.
- Threading: поток контроллеров controller_manager.
- Authority: `joint_state_broadcaster` — источник истины о положении суставов;
  см. [ADR-004](../decisions/ADR-004-single-ros2-control-source.md).

## Точки изменения

| Что нужно поменять | Где править |
|---|---|
| Точку назначения демо-движения | `arm_control/apps/*.cpp` |
| Группу планирования, фреймы, позу home | `arm_control/config/motion.yaml` |
| Планировщик (OMPL → PILZ) | `arm_moveit_config/config/*planning*.yaml` |
| Решатель IK | `arm_moveit_config/config/kinematics.yaml` |
| Геометрию и лимиты робота | `arm_description/urdf/arm.urdf.xacro` |
| Порядок запуска узлов | `arm_description/launch/*.launch.py` |
| Часы планировщика под симуляцией | `arm_moveit_config/launch/gazebo_demo.launch.py` |

## Точки входа

```bash
colcon build --symlink-install && source install/setup.bash

ros2 launch arm_description display.launch.py          # модель + ползунки суставов + RViz
ros2 launch arm_description gazebo.launch.py           # симуляция Gazebo, без планирования
ros2 launch arm_moveit_config demo.launch.py           # MoveIt + mock ros2_control + RViz
ros2 launch arm_moveit_config gazebo_demo.launch.py    # MoveIt поверх Gazebo (часы симуляции)

ros2 launch arm_control move_joints.launch.py          # движение по суставным целям
ros2 launch arm_control move_tool_pose.launch.py       # движение по позе инструмента

colcon test --packages-select arm_control arm_description
```

Приложения `arm_control` требуют запущенного `move_group`: только он публикует
SRDF. С `gazebo_demo.launch.py` клиенту нужен ещё и `use_sim_time:=true`:

```bash
ros2 launch arm_control move_tool_pose.launch.py use_sim_time:=true
```

См. [ADR-006](../decisions/ADR-006-simulation-clock.md).
