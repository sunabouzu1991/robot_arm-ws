"""
Контракт helper-модуля описания робота.

URDF всегда без XML-комментариев, а режим Gazebo добавляет хардварные
интерфейсы gazebo_ros2_control. Запуск: colcon test --packages-select arm_description.
"""

from arm_description.robot_description import load_robot_description, urdf_path

GAZEBO_HARDWARE = 'gazebo_ros2_control/GazeboSystem'


def test_urdf_file_exists():
    assert urdf_path().endswith('arm.urdf.xacro')


def test_plain_description_has_no_comments():
    urdf = load_robot_description()

    assert '<robot' in urdf
    assert '<!--' not in urdf


def test_plain_description_keeps_hardware_to_moveit_config():
    """Без use_gazebo интерфейсы объявляет arm_moveit_config, а не описание."""
    urdf = load_robot_description()

    assert GAZEBO_HARDWARE not in urdf


def test_gazebo_description_declares_gazebo_hardware():
    urdf = load_robot_description(use_gazebo=True)

    assert GAZEBO_HARDWARE in urdf
    assert '<!--' not in urdf
