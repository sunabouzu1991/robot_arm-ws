"""
Просмотр модели: robot_state_publisher + ползунки суставов + RViz.

Читается сверху вниз: что публикуем, чем крутим суставы, чем смотрим.
"""

from launch import LaunchDescription
from launch_ros.actions import Node

from arm_description.robot_description import load_robot_description, share_path

RVIZ_CONFIG = share_path('rviz', 'view_robot.rviz')


def generate_launch_description():
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{'robot_description': load_robot_description()}],
    )

    joint_state_publisher_gui = Node(
        package='joint_state_publisher_gui',
        executable='joint_state_publisher_gui',
        name='joint_state_publisher_gui',
        output='screen',
    )

    rviz = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', RVIZ_CONFIG],
    )

    return LaunchDescription([robot_state_publisher, joint_state_publisher_gui, rviz])
