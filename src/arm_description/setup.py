import os
from glob import glob
from setuptools import find_packages, setup

package_name = 'arm_description'

setup(
    name=package_name,
    version='0.1.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'), glob('launch/*.py')),
        (os.path.join('share', package_name, 'urdf'), glob('urdf/*.*')),
        (os.path.join('share', package_name, 'config'), glob('config/*.*')),
        (os.path.join('share', package_name, 'rviz'), glob('rviz/*.*')),
        (os.path.join('share', package_name, 'worlds'), glob('worlds/*.*')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='aset',
    maintainer_email='rahmgalievaset@gmail.com',
    description='Описание 6-DOF манипулятора arm: модель, мир Gazebo, RViz',
    license='Apache-2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [],
    },
)
