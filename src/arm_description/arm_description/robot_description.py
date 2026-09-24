"""
Загрузка описания робота для launch-файлов пакета arm_description.

Один источник истины для описания робота: и display.launch.py, и gazebo.launch.py
берут URDF отсюда, поэтому правила обработки xacro описаны в одном месте.

Ответственность: выдать строку URDF, пригодную для параметра robot_description.
Владеет: путями к ресурсам пакета и правилом очистки XML-комментариев.
Читает: файл urdf/arm.urdf.xacro из share-каталога пакета.
Пишет: ничего.
Не знает: о Gazebo, RViz, контроллерах и узлах launch.
"""

import os
import re

from ament_index_python.packages import get_package_share_directory
import xacro

PACKAGE_NAME = 'arm_description'
URDF_FILE_NAME = 'arm.urdf.xacro'

_XML_COMMENT = re.compile(r'<!--.*?-->', re.S)


def urdf_path() -> str:
    """Абсолютный путь к xacro-файлу описания робота в установленном пакете."""
    return share_path('urdf', URDF_FILE_NAME)


def share_path(*parts: str) -> str:
    """Абсолютный путь к ресурсу внутри share-каталога установленного пакета."""
    return os.path.join(get_package_share_directory(PACKAGE_NAME), *parts)


def load_robot_description(use_gazebo: bool = False) -> str:
    """
    Возвращает URDF-строку для параметра robot_description.

    use_gazebo=True добавляет хардварные интерфейсы gazebo_ros2_control.

    Комментарии вырезаются намеренно: gazebo_ros2_control (Humble) прокидывает URDF
    в rcl командой `--param robot_description:=<urdf>`, а rcl разбирает значение как
    YAML. Последовательность ": " внутри XML-комментария делает строку невалидным
    plain-scalar'ом ("mapping values are not allowed here"), и controller_manager
    не поднимается.
    """
    mappings = {'use_gazebo': 'true' if use_gazebo else 'false'}
    urdf = xacro.process_file(urdf_path(), mappings=mappings).toxml()
    return _XML_COMMENT.sub('', urdf)
