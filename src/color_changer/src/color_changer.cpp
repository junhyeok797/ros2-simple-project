#include "color_changer.hpp"


ColorChanger::ColorChanger(const std::string& node_name, const double& loop_rate) : Node(node_name)
{
    auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10));

    p_turtle_color_ = this->create_publisher<my_msgs::msg::TurtleColor>("turtle_color", qos_profile);
    s_turtle_pose_ = this->create_subscription<turtlesim::msg::Pose>(
        "/turtle1/pose", qos_profile, std::bind(&ColorChanger::CallbackTurtlePose, this, std::placeholders::_1));
    
    params_.push_back(rclcpp::Parameter("background_r", 255));
    params_.push_back(rclcpp::Parameter("background_g", 0));
    params_.push_back(rclcpp::Parameter("background_b", 0));

    t_run_node_ = this->create_wall_timer(
        std::chrono::milliseconds((int64_t)(1000 / loop_rate)),
        [this]()
        { this->Run(this->now()); });
}

void ColorChanger::Run(const rclcpp::Time& current_time)
{
    double yaw_deg = i_turtle_pose_.theta * 180.0 / M_PI;
    o_turtle_color_.r = 0.0;
    o_turtle_color_.g = 0.0;
    o_turtle_color_.b = 0.0;

    param_client_ = std::make_shared<rclcpp::AsyncParametersClient>(this, "/turtlesim");

    if (yaw_deg >= 0.0 && yaw_deg < 90.0)
    {
        params_.clear();
        params_.push_back(rclcpp::Parameter("background_r", 255));
        params_.push_back(rclcpp::Parameter("background_g", 255));
        params_.push_back(rclcpp::Parameter("background_b", 255));
        param_client_->set_parameters(params_);
        o_turtle_color_.r = 1.0;
        o_turtle_color_.g = 1.0;
        o_turtle_color_.b = 1.0;
    }
    else if (yaw_deg >= 90.0 && yaw_deg < 180.0)
    {
        params_.clear();
        params_.push_back(rclcpp::Parameter("background_r", 255));
        params_.push_back(rclcpp::Parameter("background_g", 0));
        params_.push_back(rclcpp::Parameter("background_b", 0));
        param_client_->set_parameters(params_);
        o_turtle_color_.r = 1.0;
    }
    else if (yaw_deg >= -180.0 && yaw_deg < -90.0)
    {
        params_.clear();
        params_.push_back(rclcpp::Parameter("background_r", 0));
        params_.push_back(rclcpp::Parameter("background_g", 255));
        params_.push_back(rclcpp::Parameter("background_b", 0));
        param_client_->set_parameters(params_);
        o_turtle_color_.g = 1.0;
    }
    else if (yaw_deg >= -90.0 && yaw_deg < 0.0)
    {
        params_.clear();
        params_.push_back(rclcpp::Parameter("background_r", 0));
        params_.push_back(rclcpp::Parameter("background_g", 0));
        params_.push_back(rclcpp::Parameter("background_b", 255));
        param_client_->set_parameters(params_);
        o_turtle_color_.b = 1.0;
    }

    Publish(current_time);
}

void ColorChanger::Publish(const rclcpp::Time& current_time)
{
    p_turtle_color_->publish(o_turtle_color_);
}


int main(int argc, char* argv[])
{
    std::string node_name = "color_changer";
    double loop_rate = 10.0;

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ColorChanger>(node_name, loop_rate));
    rclcpp::shutdown();

    return 0;
}