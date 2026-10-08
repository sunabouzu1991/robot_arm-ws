---
description: 'Правила написания и запуска тестов в воркспейсе.'
applyTo: "**/test/**"
---
# Тесты

## Фреймворки
- Python: `python3-pytest`. Статический анализ: `ament_flake8`, `ament_pep257`.
- C++: `ament_cmake_gtest`.

## Обязательные границы
- Тесты `arm_control/domain` и `arm_control/application` **не должны** требовать запущенного `move_group` или ROS-графа.
- Тесты `arm_description` проверяют `robot_description.py`: отсутствие `<!--`, наличие `gazebo_ros2_control` только при `use_gazebo=true`.

## Запуск
```bash
colcon test --packages-select arm_control arm_description
colcon test-result --verbose
```

## Что считается «прошло»
- Все заявленные тесты запущены (не пропущены).
- Тесты не ослаблены: если ассерт пришлось изменить, это отдельное решение, а не «подгонка».
- Проверяются наблюдаемые результаты, а не факт выполнения функции.