"""
Планирование MoveIt поверх симуляции Gazebo: демонстрация в одном запуске.

Читается сверху вниз: аргументы -> симулятор -> планировщик -> визуализация.
Симулятор целиком принадлежит пакету arm_description, здесь только его подключение.

Ответственность: связать контроллеры Gazebo с планировщиком MoveIt.
Владеет: узлами move_group и rviz2.
Читает: config/*.yaml пакета arm_moveit_config через MoveItConfigsBuilder.
Пишет: ничего.
Не знает: какие цели движения запросит клиент — это пакет arm_control.
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from moveit_configs_utils import MoveItConfigsBuilder

DESCRIPTION_PACKAGE = 'arm_description'
MOVIT_CONFIG_PACKAGE = 'arm_moveit_config'
PLANNING_GROUP = 'arm'
RVIZ_CONFIG = PathJoinSubstitution(
    [FindPackageShare(MOVIT_CONFIG_PACKAGE), 'config', 'moveit.rviz']
)

# move_group по умолчанию живёт на системных часах, а контроллеры под Gazebo —
# на часах симуляции. Метки времени траектории уезжают в прошлое, arm_controller
# отменяет исполнение и клиент получает CONTROL_FAILED.
SIMULATION_CLOCK = {'use_sim_time': True}


def declare_gui_argument():
    return DeclareLaunchArgument(
        'gui',
        default_value='true',
        description='Клиент Gazebo: false — только сервер (headless)',
    )


def declare_rviz_argument():
    return DeclareLaunchArgument(
        'use_rviz',
        default_value='true',
        description='false — без визуализации сцены',
    )


def simulation():
    return IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            [FindPackageShare(DESCRIPTION_PACKAGE), '/launch/gazebo.launch.py']
        ),
        launch_arguments={'gui': LaunchConfiguration('gui')}.items(),
    )


def move_group(moveit_config):
    return Node(
        package='moveit_ros_move_group',
        executable='move_group',
        output='screen',
        parameters=[
            moveit_config.to_dict(),
            # Клиенты планирования забирают SRDF у самого move_group.
            {'publish_robot_description_semantic': True},
            SIMULATION_CLOCK,
        ],
    )


def rviz(moveit_config):
    return Node(
        package='rviz2',
        executable='rviz2',
        output='log',
        arguments=['-d', RVIZ_CONFIG],
        parameters=[
            moveit_config.planning_pipelines,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
            SIMULATION_CLOCK,
        ],
        condition=IfCondition(LaunchConfiguration('use_rviz')),
    )


def generate_launch_description():
    moveit_config = MoveItConfigsBuilder(
        PLANNING_GROUP, package_name=MOVIT_CONFIG_PACKAGE
    ).to_moveit_configs()

    return LaunchDescription([
        declare_gui_argument(),
        declare_rviz_argument(),
        simulation(),
        move_group(moveit_config),
        rviz(moveit_config),
    ])
