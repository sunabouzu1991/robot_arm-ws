"""
Симуляция в Gazebo: мир, robot_state_publisher, спавн модели, активация контроллеров.

Читается сверху вниз: описание -> сцена -> спавн -> контроллеры.
Каждый шаг — отдельная маленькая функция, чтобы точка изменения была видна сразу.

Владеет: порядком запуска узлов и связкой spawn -> broadcaster -> arm_controller.
Читает: urdf/arm.urdf.xacro, worlds/arm_world.world из share-каталога пакета.
Пишет: ничего (только параметры и аргументы запуска).
Не знает: о планировании движения — это пакет arm_control.
"""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    IncludeLaunchDescription,
    RegisterEventHandler,
    SetEnvironmentVariable,
)
from launch.event_handlers import OnProcessExit
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

from arm_description.robot_description import load_robot_description, share_path

ROBOT_ENTITY_NAME = 'arm'
CONTROLLER_MANAGER_TIMEOUT_S = '120'
WORLD_FILE = share_path('worlds', 'arm_world.world')
GAZEBO_SHARE = get_package_share_directory('gazebo_ros')
GAZEBO_LAUNCH_FILE = os.path.join(GAZEBO_SHARE, 'launch', 'gazebo.launch.py')

# gzserver при старте синхронно опрашивает онлайн-базу моделей (models.gazebosim.org)
# и в контейнерах или за прокси висит на этом запросе бесконечно: мир не грузится,
# сервис /spawn_entity не появляется, spawn_entity.py падает по таймауту.
# Пустое значение запрещает поход в сеть — берутся локальные модели
# из /usr/share/gazebo-11/models (sun и ground_plane там есть).
DISABLE_ONLINE_MODEL_DATABASE = SetEnvironmentVariable('GAZEBO_MODEL_DATABASE_URI', '')


def declare_gui_argument():
    return DeclareLaunchArgument(
        'gui',
        default_value='true',
        description='Клиент Gazebo: false — только сервер (headless)',
    )


def gazebo_simulation():
    return IncludeLaunchDescription(
        PythonLaunchDescriptionSource(GAZEBO_LAUNCH_FILE),
        launch_arguments={'gui': LaunchConfiguration('gui'), 'world': WORLD_FILE}.items(),
    )


def robot_state_publisher():
    return Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[
            {'robot_description': load_robot_description(use_gazebo=True), 'use_sim_time': True},
        ],
    )


def spawn_robot():
    return Node(
        package='gazebo_ros',
        executable='spawn_entity.py',
        arguments=['-topic', 'robot_description', '-entity', ROBOT_ENTITY_NAME],
        output='screen',
    )


def controller_spawner(controller_name):
    return Node(
        package='controller_manager',
        executable='spawner',
        arguments=[controller_name, '--controller-manager-timeout', CONTROLLER_MANAGER_TIMEOUT_S],
        output='screen',
    )


def generate_launch_description():
    spawn_entity = spawn_robot()
    joint_state_broadcaster = controller_spawner('joint_state_broadcaster')
    arm_controller = controller_spawner('arm_controller')

    return LaunchDescription([
        declare_gui_argument(),
        DISABLE_ONLINE_MODEL_DATABASE,
        gazebo_simulation(),
        robot_state_publisher(),
        spawn_entity,
        # ros2_control поднимает контроллеры только вместе с моделью, поэтому цепочка:
        # спавн -> broadcaster (источник /joint_states) -> arm_controller.
        RegisterEventHandler(
            OnProcessExit(target_action=spawn_entity, on_exit=[joint_state_broadcaster])
        ),
        RegisterEventHandler(
            OnProcessExit(target_action=joint_state_broadcaster, on_exit=[arm_controller])
        ),
    ])
