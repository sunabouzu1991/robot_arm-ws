# ADR-004: один блок ros2_control на оба режима (Gazebo и mock)

## Контекст

ros2_control описывался в URDF дважды: отдельный блок для Gazebo и отдельный для
MoveIt/mock. Блоки расходились (имена суставов, порядок интерфейсов), и при запуске
Gazebo подхватывались оба описания → `controller_manager` видел два конфликтующих
хардвара и не поднимал `arm_controller`.

## Решение

Один блок `ros2_control` в `arm_description/urdf/arm.urdf.xacro`, включаемый
аргументом xacro:

```xml
<xacro:arg name="use_gazebo" default="false"/>
...
<xacro:if value="$(arg use_gazebo)">
  <plugin>gazebo_ros2_control/GazeboSystem</plugin>
</xacro:if>
<xacro:unless value="$(arg use_gazebo)">
  <plugin>mock_components/GenericSystem</plugin>
</xacro:unless>
```

`display.launch.py` грузит описание с `use_gazebo=false` (mock), `gazebo.launch.py` —
с `use_gazebo=true`. Единственный потребитель режима — helper
`arm_description/robot_description.py::load_robot_description`.

## Последствия

- Невозможен запуск с двумя хардварами сразу: режим задаётся одним флагом.
- Joins-лимиты и интерфейсы описаны один раз, расхождение невозможно.
- Одновременно MoveIt/mock и Gazebo запускать нельзя: будет два
  `controller_manager`. Это осознанное ограничение, а не баг.

## Отклонённые альтернативы

- Два отдельных xacro-файла для Gazebo и mock — снова дублирование одних и тех же
  суставов и лимитов.
- Всегда использовать `gazebo_ros2_control` — не работает без Gazebo.
