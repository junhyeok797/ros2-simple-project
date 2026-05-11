#include "moving_turtle.hpp"


MovingTurtle::MovingTurtle(const std::string& node_name, const double& loop_rate) : Node(node_name)
{
    p_turtle_cmd_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

    t_run_node_ = this->create_wall_timer(
        std::chrono::milliseconds((int64_t)(1000 / loop_rate)),
        [this]()
        { this->Run(this->now()); });
}

void MovingTurtle::Run(const rclcpp::Time& current_time)
{
    double vx;
    double yawrate;

    if ((loop_count_ / 100) % 2 == 0) {
        vx = 2.0;
        yawrate = 0.0;
    }
    else if ((loop_count_ / 100) % 2 == 1) {
        vx = 0.0;
        yawrate = 1.0;
    }

    loop_count_++;

    o_turtle_cmd_.linear.x = vx;
    o_turtle_cmd_.angular.z = yawrate;

    Publish(current_time);
}

void MovingTurtle::Publish(const rclcpp::Time& current_time)
{
    p_turtle_cmd_->publish(o_turtle_cmd_);
}


int main(int argc, char* argv[])
{
    std::string node_name = "moving_turtle";
    double loop_rate = 100.0;

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MovingTurtle>(node_name, loop_rate));
    rclcpp::shutdown();

    return 0;
}