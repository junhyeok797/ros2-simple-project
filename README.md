# ros2-simple-project

ROS 2 기반의 간단한 turtlesim 예제 프로젝트입니다. 거북이를 자동으로 움직이고, 거북이의 자세(yaw)에 따라 배경색을 변경하며, RViz2에서 위치를 마커로 시각화합니다.

## 개요

본 프로젝트는 ROS 2의 기본 개념(Publisher/Subscriber, Parameter, Launch, Custom Message, RViz 시각화)을 함께 학습할 수 있도록 구성된 멀티 패키지 워크스페이스입니다.

### 전체 데이터 흐름

```
                ┌──────────────────┐
                │  moving_turtle   │
                │  (cmd_vel 100Hz) │
                └────────┬─────────┘
                         │ /turtle1/cmd_vel
                         ▼
                ┌──────────────────┐
                │    turtlesim     │
                └────────┬─────────┘
                         │ /turtle1/pose
            ┌────────────┴────────────┐
            ▼                         ▼
  ┌──────────────────┐      ┌──────────────────┐
  │  color_changer   │      │       hmi        │
  │  (10Hz)          │      │  (visualizer)    │
  └────────┬─────────┘      └────────┬─────────┘
           │ turtle_color             │ turtle_marker
           ├──────────────────────────►
           │ (param set)              ▼
           ▼                  ┌──────────────────┐
   turtlesim background       │      rviz2       │
                              └──────────────────┘
```

## 패키지 구성

| 패키지 | 빌드 타입 | 역할 |
|--------|-----------|------|
| [project_bringup](src/project_bringup/) | ament_cmake | 전체 노드를 한 번에 띄우는 launch 파일 제공 |
| [my_msgs](src/my_msgs/) | ament_cmake (rosidl) | 커스텀 메시지 `TurtleColor` 정의 |
| [moving_turtle](src/moving_turtle/) | ament_cmake | turtlesim에 cmd_vel을 발행해 거북이를 이동 |
| [color_changer](src/color_changer/) | ament_cmake | 거북이 yaw에 따라 turtlesim 배경색 변경 + 색상 토픽 발행 |
| [hmi](src/hmi/) | ament_cmake | 거북이 위치/색을 RViz2의 Marker로 시각화 |

### 1. project_bringup

[combined_launch.py](src/project_bringup/launch/combined_launch.py)에서 다음 5개의 노드를 한꺼번에 실행합니다.

- `turtlesim/turtlesim_node`
- `moving_turtle/moving_turtle`
- `color_changer/color_changer`
- `hmi/hmi`
- `rviz2/rviz2`

### 2. my_msgs

커스텀 메시지 [TurtleColor.msg](src/my_msgs/msg/TurtleColor.msg)를 정의합니다.

```
float32 r
float32 g
float32 b
```

### 3. moving_turtle

[moving_turtle.cpp](src/moving_turtle/src/moving_turtle.cpp)는 `100Hz`로 동작하면서 100 tick 단위로 **직진(vx=2.0)** 과 **회전(yawrate=1.0)** 을 번갈아 수행하는 `geometry_msgs/Twist` 메시지를 `/turtle1/cmd_vel`에 발행합니다.

- Publisher: `/turtle1/cmd_vel` (`geometry_msgs/msg/Twist`)

### 4. color_changer

[color_changer.cpp](src/color_changer/src/color_changer.cpp)는 `10Hz`로 동작하며, `/turtle1/pose`를 구독하여 거북이의 yaw 각도에 따라 turtlesim의 배경색 파라미터를 변경하고, 동일한 색 정보를 `turtle_color` 토픽으로도 발행합니다.

| yaw 범위 (deg)   | 배경색 |
|------------------|--------|
| `[0, 90)`        | 흰색 (255, 255, 255) |
| `[90, 180)`      | 빨강 (255, 0, 0) |
| `[-180, -90)`    | 초록 (0, 255, 0) |
| `[-90, 0)`       | 파랑 (0, 0, 255) |

- Subscriber: `/turtle1/pose` (`turtlesim/msg/Pose`)
- Publisher: `turtle_color` (`my_msgs/msg/TurtleColor`)
- Parameter Client: `/turtlesim` 노드의 `background_r/g/b`

### 5. hmi

[visualizer.cpp](src/hmi/src/visualizer.cpp)는 `10Hz`로 동작하며, 거북이의 위치와 색을 구독해 RViz2에서 볼 수 있는 `visualization_msgs/Marker`(CUBE) 메시지를 발행합니다.

- Subscriber: `/turtle1/pose`, `turtle_color`
- Publisher: `turtle_marker` (`visualization_msgs/msg/Marker`)
- Marker frame: `map`

## 디렉토리 구조

```
ros2-simple-project/
├── .gitignore
└── src/
    ├── project_bringup/
    │   ├── launch/combined_launch.py
    │   ├── CMakeLists.txt
    │   └── package.xml
    ├── my_msgs/
    │   ├── msg/TurtleColor.msg
    │   ├── CMakeLists.txt
    │   └── package.xml
    ├── moving_turtle/
    │   ├── include/moving_turtle/moving_turtle.hpp
    │   ├── src/moving_turtle.cpp
    │   ├── CMakeLists.txt
    │   └── package.xml
    ├── color_changer/
    │   ├── include/color_changer/color_changer.hpp
    │   ├── src/color_changer.cpp
    │   ├── CMakeLists.txt
    │   └── package.xml
    └── hmi/
        ├── include/hmi/visualizer.hpp
        ├── src/visualizer.cpp
        ├── CMakeLists.txt
        └── package.xml
```

## 요구 사항

- ROS 2 (turtlesim, rviz2, visualization_msgs 포함)
- `colcon` 빌드 도구
- C++17 이상 컴파일러 (g++ 또는 clang)

## 빌드 방법

워크스페이스 루트에서 다음을 실행합니다.

```bash
# ROS 2 환경 소싱
source /opt/ros/$ROS_DISTRO/setup.bash

# 빌드
colcon build --symlink-install

# 빌드된 워크스페이스 소싱
source install/setup.bash
```

## 실행 방법

`project_bringup`의 launch 파일을 실행하면 turtlesim, moving_turtle, color_changer, hmi, rviz2가 모두 한 번에 실행됩니다.

```bash
ros2 launch project_bringup combined_launch.py
```

RViz2에서 다음을 설정하면 마커를 확인할 수 있습니다.

- `Fixed Frame`을 `map`으로 설정
- `Add` → `Marker`를 추가하고 토픽을 `/turtle_marker`로 지정

각 노드를 개별 실행하려면 다음과 같이 사용합니다.

```bash
ros2 run turtlesim turtlesim_node
ros2 run moving_turtle moving_turtle
ros2 run color_changer color_changer
ros2 run hmi hmi
ros2 run rviz2 rviz2
```

## 토픽 / 파라미터 요약

| 토픽 | 타입 | 발행자 | 구독자 |
|------|------|--------|--------|
| `/turtle1/cmd_vel` | `geometry_msgs/Twist` | moving_turtle | turtlesim |
| `/turtle1/pose`    | `turtlesim/Pose`      | turtlesim     | color_changer, hmi |
| `turtle_color`     | `my_msgs/TurtleColor` | color_changer | hmi |
| `turtle_marker`    | `visualization_msgs/Marker` | hmi   | rviz2 |

| 파라미터 (노드 `/turtlesim`) | 타입 | 설명 |
|------------------------------|------|------|
| `background_r` | int | 배경 R 값 |
| `background_g` | int | 배경 G 값 |
| `background_b` | int | 배경 B 값 |

## 메인테이너

- Junhyeok Seo (`jun2342@chungbuk.ac.kr`)
