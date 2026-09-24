#include "arm_control/adapters/moveit_session.hpp"

namespace arm_control::adapters
{

MoveItSession::MoveItSession(const std::string& node_name)
{
  // Переопределения параметров из командной строки (в том числе те, что
  // подставляет move_group) должны объявляться автоматически.
  rclcpp::NodeOptions options;
  options.automatically_declare_parameters_from_overrides(true);

  node_ = std::make_shared<rclcpp::Node>(node_name, options);
  executor_.add_node(node_);
  spin_thread_ = std::thread([this]() { executor_.spin(); });
}

MoveItSession::~MoveItSession()
{
  executor_.cancel();
  if (spin_thread_.joinable()) {
    spin_thread_.join();
  }
}

}  // namespace arm_control::adapters
