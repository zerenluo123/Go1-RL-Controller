//
// Created by zixin on 11/1/21.
//
// stl
#include <iostream>
#include <iomanip>
#include <memory>
#include <thread>
#include <chrono>

// ROS
#include <ros/ros.h>
#include <ros/console.h>
#include <sensor_msgs/Imu.h>

//#include "torch_eigen/TorchEigen.hpp"
#include "Go1RLController.hpp"


int main(int argc, char **argv) {
  ros::init(argc, argv, "gazebo_go1_rl_ctrl");
  ros::NodeHandle nh;


  // make sure the ROS infra using sim time, otherwise the controller cannot run with correct time steps
  bool use_sim_time;
  if (ros::param::get("use_sim_time", use_sim_time)) {
    if (!use_sim_time) {
      ROS_WARN_STREAM(" ROS must set use_sim_time in order to use this program! ");
      return -1;
    }
  }

  // create go1 rl controller
  std::unique_ptr<Go1RLController> go1_rl = std::make_unique<Go1RLController>(nh);
  go1_rl->create(0.1);

  std::atomic<bool> control_execute{};
  control_execute.store(true, std::memory_order_release);

  // Thread 1: compute command, forward the NN policy network
  std::cout << "Enter thread 1: compute desired action command" << std::endl;
  std::thread compute_action_thread([&]() {
    while (control_execute.load(std::memory_order_acquire) && ros::ok()) {
      ros::Duration(ACTION_UPDATE_FREQUENCY / 1000).sleep();

      auto t1 = std::chrono::high_resolution_clock::now();

      // compute actions
      bool running = go1_rl->advance(0.1);

      auto t2 = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> ms_double = t2 - t1;
      std::cout << "RL solution is updated in " << ms_double.count() << "ms" << std::endl;

      if (!running) {
        std::cout << "Thread 1 loop is terminated because of errors." << std::endl;
        ros::shutdown();
        std::terminate();
        break;
      }
    }

  });

//  // Thread 2: depolyment on the real system; send commands
//  std::cout << "Enter thread 2: Deployment on the real robot, sending command" << std::endl;
//  std::thread deploy_thread([&]() {
//    while (control_execute.load(std::memory_order_acquire) && ros::ok()) {
//      ros::Duration(DEPLOYMENT_FREQUENCY / 1000).sleep();
//
//      bool send_cmd_running = go1_rl->send_cmd();
//
//      if (!send_cmd_running) {
//        std::cout << "Thread 2 loop is terminated because of errors." << std::endl;
//        ros::shutdown();
//        std::terminate();
//        break;
//      }
//
//    }
//  });

  ros::AsyncSpinner spinner(12);
  spinner.start();

  compute_action_thread.join();
//  deploy_thread.join();

  return 0;

}
