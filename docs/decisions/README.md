# Architecture Decision Records

Решения фиксируются здесь, чтобы через полгода (или агенту в новой сессии) было
понятно не только «как сделано», но и «почему нельзя иначе».

| ADR | Решение | Статус |
|---|---|---|
| [ADR-001](ADR-001-package-split.md) | Разделение воркспейса на `arm_description` / `arm_moveit_config` / `arm_control` | accepted |
| [ADR-002](ADR-002-layered-arm-control.md) | Слои domain / ports / application / adapters в `arm_control` | accepted |
| [ADR-003](ADR-003-motion-settings.md) | `config/motion.yaml` — единственный источник настроек движения | accepted |
| [ADR-004](ADR-004-single-ros2-control-source.md) | Один блок `ros2_control`, режим задаётся аргументом `use_gazebo` | accepted |
| [ADR-005](ADR-005-gazebo-launch-pitfalls.md) | Очистка XML-комментариев и запрет онлайн-базы моделей Gazebo | accepted |
| [ADR-006](ADR-006-simulation-clock.md) | `move_group` поверх Gazebo работает на часах симуляции | accepted |

## Формат

```text
# ADR-NNN: краткое название
## Контекст   — что было и почему это мешало
## Решение    — что решили, с фрагментами кода
## Последствия — что теперь можно, чего нельзя, чем платим
## Отклонённые альтернативы — что рассматривали и почему не подошло
```

Новый ADR не редактирует старый: решение меняется новым ADR со ссылкой на
предыдущий.
