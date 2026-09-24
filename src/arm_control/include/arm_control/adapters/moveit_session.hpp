#pragma once

#include <string>
#include <thread>

#include <rclcpp/rclcpp.hpp>

namespace arm_control::adapters
{

/// Владеет узлом rclcpp и фоновым спиннером: без спиннера MoveGroupInterface
/// не получает ответы action-серверов и любой план уходит в таймаут.
/// Время жизни — оболочка приложения; узел наружу отдаётся только на чтение.
class MoveItSession
{
public:
  explicit MoveItSession(const std::string& node_name);
  ~MoveItSession();

  MoveItSession(const MoveItSession&) = delete;
  MoveItSession& operator=(const MoveItSession&) = delete;

  const rclcpp::Node::SharedPtr& node() const { return node_; }
  rclcpp::Logger logger() const { return node_->get_logger(); }

private:
  rclcpp::Node::SharedPtr node_;
  rclcpp::executors::SingleThreadedExecutor executor_;
  std::thread spin_thread_;
};

}  // namespace arm_control::adapters
