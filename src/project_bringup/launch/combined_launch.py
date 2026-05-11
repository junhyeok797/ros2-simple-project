import os

from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    turtlesim = Node(
        package='turtlesim',
        executable='turtlesim_node',
        name='turtlesim',
        output='screen',
    )
    moving_turtle = Node(
        package='moving_turtle',
        executable='moving_turtle',
        name='moving_turtle',
        output='screen',
    )
    color_changer = Node(
        package='color_changer',
        executable='color_changer',
        name='color_changer',
        output='screen',
    )
    hmi = Node(
        package='hmi',
        executable='hmi',
        name='hmi',
        output='screen',
    )
    rviz2 = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
    )

    return LaunchDescription([
        turtlesim,
        moving_turtle,
        color_changer,
        hmi,
        rviz2,
    ])
