
#include <limits>
#include <semantic_components/semantic_component_interface.hpp>
#include <sensor_msgs/msg/battery_state.hpp>

namespace netizen_robotics
{
    class BatterySensor : public semantic_components::SemanticComponentInterface<sensor_msgs::msg::BatteryState>
    {
    public:
        explicit BatterySensor(const std::string &name, uint8_t cell_count = 1)
            : semantic_components::SemanticComponentInterface<sensor_msgs::msg::BatteryState>(name, 2 * cell_count + 8)
        {
            cell_count_ = cell_count;
            interface_names_.emplace_back(name_ + "/" + "voltage");
            interface_names_.emplace_back(name_ + "/" + "current");
            interface_names_.emplace_back(name_ + "/" + "charge");
            interface_names_.emplace_back(name_ + "/" + "capacity");
            interface_names_.emplace_back(name_ + "/" + "percentage");
            interface_names_.emplace_back(name_ + "/" + "power_supply_status");
            interface_names_.emplace_back(name_ + "/" + "power_supply_health");
            interface_names_.emplace_back(name_ + "/" + "present");
            for (uint i = 1u; i <= cell_count_; i++)
                interface_names_.emplace_back(name_ + "/" + "cell_voltage_" + std::to_string(i));
            for (uint i = 1u; i <= cell_count_; i++)
                interface_names_.emplace_back(name_ + "/" + "cell_temperature_" + std::to_string(i));
        }

        virtual ~BatterySensor() = default;

        bool get_values_as_message(sensor_msgs::msg::BatteryState &message)
        {
            message.voltage = static_cast<float>(state_interfaces_[0].get().get_value());
            message.current = static_cast<float>(state_interfaces_[1].get().get_value());
            message.charge = static_cast<float>(state_interfaces_[2].get().get_value());
            message.capacity = static_cast<float>(state_interfaces_[3].get().get_value());
            message.percentage = static_cast<float>(state_interfaces_[4].get().get_value());
            message.power_supply_status = static_cast<uint8_t>(state_interfaces_[5].get().get_value());
            message.power_supply_health = static_cast<uint8_t>(state_interfaces_[6].get().get_value());
            message.present = state_interfaces_[7].get().get_value() > 0.0;
            uint index = 8;
            for (uint i = 0u; i < cell_count_; i++)
            {
                message.cell_voltage[i] = static_cast<float>(state_interfaces_[index + i].get().get_value());
            }
            index += cell_count_;
            for (uint i = 0u; i < cell_count_; i++)
            {
                message.cell_temperature[i] = static_cast<float>(state_interfaces_[index + i].get().get_value());
            }
            return true;
        }

    private:
        uint cell_count_ = 1;
    };
} // namespace netizen_robotics