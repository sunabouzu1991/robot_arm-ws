"""
Контракт helper-модуля описания робота.

URDF всегда без XML-комментариев, а режим Gazebo добавляет хардварные
интерфейсы gazebo_ros2_control. Запуск: colcon test --packages-select arm_description.
"""

from math import isclose
from xml.etree import ElementTree

from arm_description.robot_description import load_robot_description, urdf_path

GAZEBO_HARDWARE = 'gazebo_ros2_control/GazeboSystem'
TARGET_PAYLOAD_KG = 0.5
TARGET_MODEL_MASS_KG = 4.15
TARGET_NEUTRAL_TCP_HEIGHT_M = 0.5
TARGET_SHOULDER_TO_TCP_REACH_M = 0.28
TARGET_PAYLOAD_TO_ROBOT_MASS_RATIO = 0.1
MAX_PAYLOAD_TO_ROBOT_MASS_RATIO = 0.2


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


def test_compact_model_meets_reach_and_payload_mass_targets():
    robot = ElementTree.fromstring(load_robot_description())
    joint_offsets = {
        joint.get('name'): float(joint.find('origin').get('xyz').split()[2])
        for joint in robot.findall('joint')
        if joint.find('origin') is not None
    }
    robot_mass_kg = sum(
        float(mass.get('value'))
        for mass in robot.findall('./link/inertial/mass')
    )
    shoulder_to_tcp_reach_m = sum(
        joint_offsets[name]
        for name in ('joint_3', 'joint_4', 'joint_5', 'joint_6', 'joint_tcp')
    )
    payload_to_mass_ratio = TARGET_PAYLOAD_KG / robot_mass_kg

    assert isclose(sum(joint_offsets.values()), TARGET_NEUTRAL_TCP_HEIGHT_M, abs_tol=1e-9)
    assert isclose(shoulder_to_tcp_reach_m, TARGET_SHOULDER_TO_TCP_REACH_M, abs_tol=1e-9)
    assert isclose(robot_mass_kg, TARGET_MODEL_MASS_KG, abs_tol=1e-9)
    assert TARGET_PAYLOAD_TO_ROBOT_MASS_RATIO <= payload_to_mass_ratio
    assert payload_to_mass_ratio <= MAX_PAYLOAD_TO_ROBOT_MASS_RATIO
