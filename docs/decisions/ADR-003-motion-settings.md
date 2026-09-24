# ADR-003: config/motion.yaml — единственный источник настроек движения

## Контекст

В старых нодах имена группы, фреймов, позы возврата, допуски и коэффициенты
масштабирования скорости были литералами внутри `main`:

```cpp
static const std::string PLANNING_GROUP = "two_link_arm";
move_group.setMaxVelocityScalingFactor(0.3);
```

Значит, при переименовании группы или фрейма нужно было править C++, а «какая
скорость у робота» не было видно без чтения кода.

## Решение

Все настройки живут в одном файле — `arm_control/config/motion.yaml` —
и читаются как параметры ROS (`readMotionSettings`). В C++ остаются только
демо-данные: точка назначения.

```yaml
/**:
  ros__parameters:
    planning_group: arm
    base_frame: base_link
    tool_frame: tcp
    home_pose_name: home
    velocity_scaling: 0.5
    acceleration_scaling: 0.5
```

Валидация — в домене (`domain::validationError`), а не в адаптере: правило
«поле обязательно, коэффициент в (0, 1]» не зависит от ROS. Отсутствие параметра
даёт ошибку с именем параметра и подсказкой, как его передать, вместо тихого
дефолта.

## Последствия

- Один grep по имени группы: `grep -rn "arm" arm_control/config/motion.yaml`.
- launch-файлы `arm_control/launch/*.launch.py` подставляют этот файл,
  поэтому настройки работают и через `ros2 run`, и через `ros2 launch`
  (при `ros2 run` нужен `--ros-args --params-file`).
- Демо-точки (`kWorkJointPositions`, `kTargetPosition`) намеренно остались в
  `apps/*.cpp`: это сценарий демонстрации, а не настройка системы.

## Отклонённые альтернативы

- YAML, читаемый в C++ вручную (без параметров ROS) — ломает `use_sim_time`,
  переопределение через `--ros-args -p` и `ros2 param set`.
- Держать настройки в `arm_moveit_config` — конфигурация планировщика и точка
  назначения приложения меняются по разным причинам; связывать их не нужно.
