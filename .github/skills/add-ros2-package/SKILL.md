---
name: add-ros2-package
description: Создаёт новый ROS 2 пакет в воркспейсе с правильной структурой package.xml, CMakeLists.txt или setup.py, тестами и зависимостями симуляции. Использовать при добавлении пакета в воркспейс arm.
argument-hint: "[имя-пакета] [ament_cmake|ament_python]"
---

# Добавление ROS 2 пакета

## Когда применять
Пользователь просит добавить новый пакет в воркспейс `arm_*`.

## Шаги

### 1. Определить тип сборки
- **ament_cmake** — C++ узлы, кастомные плагины, интеграция с MoveIt.
- **ament_python** — launch-файлы, простые узлы, конфигурационные пакеты.

### 2. Создать структуру
Для `ament_cmake`: 
  - `package_name/`
  - `CMakeLists.txt`
  - `package.xml`
  - `include/package_name/`
  - `src/`
  - `test/`

Для `ament_python`: 
  - `package_name/`
  - `package.xml`
  - `setup.py`
  - `setup.cfg`
  - `resource/package_name`
  - `package_name/`
  - `__init__.py`
  - `test/`

### 3. package.xml — обязательные правила

- Формат: `<package format="3">`.
- Зависимости сборки: `<buildtool_depend>ament_cmake</buildtool_depend>` (или `ament_python`).
- **Исполнительные зависимости для симуляции и контроллеров — только через `<exec_depend>`:**
  - `joint_trajectory_controller`
  - `controller_manager`
  - `gazebo_ros`
  - `gazebo_ros2_control`
  - `moveit_ros_move_group`
- `<depend>` — только для того, что реально нужно и на сборке, и в рантайме.
- `<test_depend>` — `ament_lint_auto`, `ament_lint_common`, `ament_cmake_gtest` (для C++) или `python3-pytest` (для Python).

### 4. CMakeLists.txt (ament_cmake)

```cmake
cmake_minimum_required(VERSION 3.16)
project(package_name)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

find_package(ament_cmake REQUIRED)
# ... find_package зависимостей ...

add_executable(node_name src/node_name.cpp)
ament_target_dependencies(node_name rclcpp ...)

install(TARGETS node_name DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
```
### 5. setup.py (ament_python)
```python
from setuptools import setup
import os
from glob import glob

package_name = 'package_name'

setup(
    name=package_name,
    version='0.0.1',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'), glob('launch/*.launch.py')),
        (os.path.join('share', package_name, 'config'), glob('config/*.yaml')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='...',
    description='...',
    license='...',
    entry_points={
        'console_scripts': [
            'node_name = package_name.node_name:main',
        ],
    },
)
```
### 6. Зависимости в CMakeLists — только если реально нужны
- Для чистых слоёв (например, домена) не вызывай `ament_target_dependencies` — это обеспечивает невозможность `#include <moveit/...>` на уровне линковки.

### 7. Тесты
- C++: `ament_cmake_gtest` в `test/`.
- Python: `python3-pytest` в `test/`.
- Линтеры: `ament_flake8`, `ament_pep257` (Python).

### 8. Проверка
```bash
colcon build --symlink-install --packages-select package_name
colcon test --packages-select package_name
colcon test-result --verbose
```
## Что нельзя
- Использовать `catkin_make` или `rosbuild`.
- Использовать `rospy`, `roscpp` — только `rclpy`, `rclcpp`.
- Объявлять `gazebo_ros` или `controller_manager` через `<depend>` или `<build_depend>` — только `<exec_depend>`.