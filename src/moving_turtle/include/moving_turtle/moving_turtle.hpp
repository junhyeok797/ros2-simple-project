#include <iostream>
#include <string>

#include "rclcpp/rclcpp.hpp"

#include "geometry_msgs/msg/twist.hpp"


class MovingTurtle : public rclcpp::Node
{
public:
    MovingTurtle(const std::string& node_name, const double& loop_rate);

private:
    // Functions
    void Run(const rclcpp::Time& current_time);
    void Publish(const rclcpp::Time& current_time);

    // Publisher
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr p_turtle_cmd_;

    // Timer
    rclcpp::TimerBase::SharedPtr t_run_node_;

    // Outputs
    geometry_msgs::msg::Twist o_turtle_cmd_;

    // Algorithm Variables
    unsigned int loop_count_ = 0;
};