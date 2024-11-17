# Battery Level Broadcaster

## Overview

The `battery_level_broadcaster` package is a ROS 2 controller that broadcasts the battery state ([BatteryState](https://docs.ros.org/en/jazzy/p/sensor_msgs/interfaces/msg/BatteryState.html)) and other related metrics of a robot. It is designed to interface with the ROS 2 control framework and provides real-time updates on the battery status.

## Features

- Broadcasts battery voltage and temperature.
- Integrates with ROS 2 control framework.
- Real-time publishing of battery state messages.

## Installation

To install the `battery_level_broadcaster` package, clone the repository into your ROS 2 workspace and build it using colcon:

```sh
cd ~/ros2_ws/src
git clone 
cd ~/ros2_ws
colcon build
```

Remember to source after building to use this package.

## Usage

To use the battery_level_broadcaster, include it in your robot's control configuration and launch the controller manager. Here is an example of how to configure and launch the broadcaster:

### Configuration

Add the configuration below to your control configuration yaml file:

```yaml
battery_level_broadcaster:
  ros__parameters:
    battery_name: "battery"
    cell_count: 3
    design_capacity: 2100.0
    power_supply_technology: 2
```

### Launch

Create a launch file to start the controller manager with the broadcaster, e.g., and add the battery broadcaster_spawner to the file:

```python
battery_broadcaster_spawner = Node(
    package="controller_manager",
    executable="spawner",
    arguments=[
        "battery_level_broadcaster",
        "--controller-manager",
        "/controller_manager",
    ],
)
```

License
This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details.

Contributing
Contributions are welcome! Please open an issue or submit a pull request.

Authors

[Danny Ng](https://github.com/dannyngweekia)
