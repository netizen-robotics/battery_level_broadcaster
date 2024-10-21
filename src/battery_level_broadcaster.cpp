#include "battery_level_broadcaster/battery_level_broadcaster.hpp"

namespace netizen_robotics
{
    controller_interface::CallbackReturn BatteryLevelBroadcaster::on_init()
    {
        try
        {
            param_listener_ = std::make_shared<battery_level_broadcaster::ParamListener>(get_node());
            params_ = param_listener_->get_params();
        }
        catch (const std::exception &e)
        {
            RCLCPP_ERROR(
                get_node()->get_logger(), "Exception thrown during init stage with message: %s \n", e.what());
            return controller_interface::CallbackReturn::ERROR;
        }

        return controller_interface::CallbackReturn::SUCCESS;
    }

    controller_interface::CallbackReturn BatteryLevelBroadcaster::on_configure(const rclcpp_lifecycle::State &)
    {
        params_ = param_listener_->get_params();
        if (params_.battery_name.empty())
        {
            RCLCPP_ERROR(get_node()->get_logger(), "'sensor_name' parameter has to be specified.");
            return CallbackReturn::ERROR;
        }
        double design_capacity = params_.design_capacity > 0 ? params_.design_capacity : std::numeric_limits<double>::quiet_NaN();
        try
        {
            battery_sensor_ = std::make_unique<BatterySensor>(BatterySensor(params_.battery_name, params_.cell_count));
            sensor_state_publisher_ = get_node()->create_publisher<sensor_msgs::msg::BatteryState>("~/battery_state", rclcpp::SystemDefaultsQoS());
            realtime_publisher_ = std::make_unique<StatePublisher>(sensor_state_publisher_);
        }
        catch (const std::exception &e)
        {
            fprintf(
                stderr, "Exception thrown during publisher creation at configure stage with message : %s \n",
                e.what());
            return CallbackReturn::ERROR;
        }

        realtime_publisher_->lock();
        realtime_publisher_->msg_.header.frame_id = params_.battery_name;
        realtime_publisher_->msg_.serial_number = params_.serial_number;
        realtime_publisher_->msg_.location = params_.location;
        realtime_publisher_->msg_.design_capacity = design_capacity;
        realtime_publisher_->msg_.power_supply_technology = params_.power_supply_technology;
        for (auto i = 0u; i < params_.cell_count; ++i)
        {
            realtime_publisher_->msg_.cell_voltage.push_back(std::numeric_limits<double>::quiet_NaN());
        }
        realtime_publisher_->unlock();

        RCLCPP_DEBUG(get_node()->get_logger(), "configure successful");

        return controller_interface::CallbackReturn::SUCCESS;
    }

    controller_interface::InterfaceConfiguration BatteryLevelBroadcaster::command_interface_configuration() const
    {
        return controller_interface::InterfaceConfiguration{controller_interface::interface_configuration_type::NONE};
    }

    controller_interface::InterfaceConfiguration BatteryLevelBroadcaster::state_interface_configuration() const
    {

        controller_interface::InterfaceConfiguration state_interfaces_config;
        state_interfaces_config.type = controller_interface::interface_configuration_type::INDIVIDUAL;
        state_interfaces_config.names = battery_sensor_->get_state_interface_names();
        return state_interfaces_config;

        return controller_interface::InterfaceConfiguration{
            controller_interface::interface_configuration_type::NONE};
    }

    controller_interface::CallbackReturn BatteryLevelBroadcaster::on_activate(const rclcpp_lifecycle::State &)
    {
        battery_sensor_->assign_loaned_state_interfaces(state_interfaces_);
        return controller_interface::CallbackReturn::SUCCESS;
    }

    controller_interface::CallbackReturn BatteryLevelBroadcaster::on_deactivate(const rclcpp_lifecycle::State &)
    {
        battery_sensor_->release_interfaces();
        return controller_interface::CallbackReturn::SUCCESS;
    }

    controller_interface::return_type BatteryLevelBroadcaster::update(const rclcpp::Time &time, const rclcpp::Duration &)
    {

        if (realtime_publisher_ && realtime_publisher_->trylock())
        {
            realtime_publisher_->msg_.header.stamp = time;
            battery_sensor_->get_values_as_message(realtime_publisher_->msg_);
            realtime_publisher_->unlockAndPublish();
        }
        return controller_interface::return_type::OK;
    }

} // namespace netizen_robotics

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(netizen_robotics::BatteryLevelBroadcaster, controller_interface::ControllerInterface)