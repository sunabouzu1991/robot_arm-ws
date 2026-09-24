---
applyTo: "**/*.py, **/CMakeLists.txt, **/package.xml"
---
# Стандарты кода и тестирование ROS 2
- Весь Python-код (launch-файлы, nodes, setup.py) должен строго проходить статический анализ линтерами `ament_flake8` и `ament_pep257`[cite: 34].
- Для модульного тестирования использовать `python3-pytest` (Python) и `ament_cmake_gtest` (C++)[cite: 34].
- В `package.xml` все исполнительные зависимости подсимуляции и контроллеров (`joint_trajectory_controller`, `controller_manager`, `gazebo_ros`) объявлять строго через тег `<exec_depend>`[cite: 34, 57].