# ADR-006: move_group поверх Gazebo работает на часах симуляции

## Контекст

`ros2 launch arm_moveit_config move_group.launch.py` (сгенерированный MoveIt
Setup Assistant) не задаёт `use_sim_time`, то есть `move_group` живёт на системных
часах, а `arm_controller` под Gazebo — на часах симуляции. Планирование проходило
успешно, а исполнение падало:

```text
[move_group_interface]: Planning request complete!
[move_group_interface]: Execute request accepted
[move_group_interface]: Execute request aborted
[ERROR] [move_tool_pose]: execution failed: ... -> MoveIt error code -4
```

`-4` — это `CONTROL_FAILED`. Метки времени траектории, посчитанные по системным
часам, для контроллера оказываются в прошлом, поэтому `arm_controller` отменяет
`FollowJointTrajectory`.

## Решение

Отдельный launch `arm_moveit_config/launch/gazebo_demo.launch.py`: симулятор
целиком берётся из `arm_description`, а `move_group` и `rviz2` поднимаются здесь
с явным флагом времени:

```python
SIMULATION_CLOCK = {'use_sim_time': True}

Node(
    package='moveit_ros_move_group',
    executable='move_group',
    parameters=[moveit_config.to_dict(), SIMULATION_CLOCK],
)
```

Запуск: `ros2 launch arm_moveit_config gazebo_demo.launch.py`
(аргументы `gui:=false`, `use_rviz:=false`).

Клиентские приложения `arm_control` обязаны использовать тот же режим:

```bash
ros2 run arm_control move_tool_pose \
  --ros-args --params-file install/arm_control/share/arm_control/config/motion.yaml \
  -p use_sim_time:=true
```

## Последствия

- Симуляция и планирование совместимы: `move_tool_pose` под Gazebo завершается
  успехом для позы инструмента и для именованной позы `home`.
- Часы стали явной частью контракта запуска: mock-режим (`demo.launch.py`) —
  системные часы, Gazebo (`gazebo_demo.launch.py`) — часы симуляции.
- Сгенерированные Setup Assistant файлы (`move_group.launch.py`,
  `moveit_rviz.launch.py`) остались без изменений: они переиспользуются только
  mock-режимом.

## Отклонённые альтернативы

- Править сгенерированный `move_group.launch.py` — файл регенерируется мастером
  настройки, правка будет потеряна.
- Выставлять `use_sim_time` в клиенте и не выставлять в планировщике — не помогает:
  время в метках траектории берёт именно `move_group`.
