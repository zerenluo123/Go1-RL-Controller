//
// Created by zerenluo on 30.09.22.
//

#include <Eigen/Core>
#include <Eigen/Dense>
#include <memory>
#include <set>
#include <chrono>
#include <map>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <fstream>

// ROS
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/JointState.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <geometry_msgs/PoseArray.h>
#include <geometry_msgs/Vector3Stamped.h>
#include <geometry_msgs/WrenchStamped.h>
#include <geometry_msgs/PointStamped.h>

// go1 hardware
#include "unitree_legged_sdk/unitree_legged_sdk.h"

#include "../Go1Params.hpp"
#include "../Go1CtrlStates.hpp"
#include "../utils/Utils.hpp"
#include "../utils/filter.hpp"
#include "../EKF/Go1BasicEKF.hpp"
#include "../legKinematics/Go1Kinematics.hpp"

#define FOOT_FILTER_WINDOW_SIZE 5

// TODO: compute the observation vector as the one in issac gym legged_robot.py
class Go1HardwareObservation{
 public:
  Go1HardwareObservation(ros::NodeHandle &nh):
      udp(UNITREE_LEGGED_SDK::LOWLEVEL, 8090, "192.168.123.10", 8007) {
    obDim_ = 36;
    obDouble_.setZero(obDim_); obScaled_.setZero(obDim_);

    // scale factor init
    linVelScale_.setZero();
    angVelScale_.setZero();
    gravityScale_.setZero();
    commandScale_.setZero();
    linVelScale_ << 2.0, 2.0, 2.0;
    angVelScale_ << 0.25, 0.25, 0.25;
    gravityScale_ << 1.0, 1.0, 1.0; // projected gravity
    commandScale_ << 2.0, 2.0, 0.25; // for cmd_velx, cmd_vely, cmd_ang_vel_yaw

    dofPosScale_.setZero(12);  dofVelScale_.setZero(12);
    for (int i = 0; i < 12; i++) {
      dofPosScale_[i] = 1.0;
      dofVelScale_[i] = 0.05;
    }
    scaleFactor_.setZero(obDim_);
    scaleFactor_ << linVelScale_, angVelScale_, gravityScale_, commandScale_, dofPosScale_, dofVelScale_;

    // joy command
    sub_joy_msg = nh.subscribe("/joy", 1000, &Go1HardwareObservation::joy_callback, this);

    joy_cmd_ctrl_state_ = 0;
    joy_cmd_ctrl_state_change_request_ = false;
    prev_joy_cmd_ctrl_state_ = 0;
    joy_cmd_exit_ = false;

    go1_ctrl_states_.reset();
    go1_ctrl_states_.resetFromROSParam(nh);

    // start hardware reading thread after everything initialized
    thread_ = std::thread(&Go1HardwareObservation::receive_low_state, this);

  }

  ~Go1HardwareObservation() { // move from the controller to here
    destruct = true;
    thread_.join();
  }

  void updateObservation(double dt) {
    // TODO: get all observation from measurement and concat
    // assign observation vector
    obDouble_.segment(0, 3) = go1_ctrl_states_.root_rot_mat_z.transpose() * go1_ctrl_states_.root_lin_vel; // 1. base linear velocity(robot frame)
    obDouble_.segment(3, 3) = go1_ctrl_states_.imu_ang_vel; // 2. base angular velocity(robot frame, [roll, pitch, yaw])
    obDouble_.segment(6, 3) = go1_ctrl_states_.root_rot_mat.transpose() * (-Eigen::Vector3d::UnitZ()); // 3. projected gravity(projected z unit vector)
    obDouble_.segment(9, 3) << joy_cmd_velx_, joy_cmd_vely_, joy_cmd_yaw_rate_; // 4. command([cmd_velx, cmd_vely, cmd_ang_vel_yaw])
    obDouble_.segment(12, 12) = go1_ctrl_states_.joint_pos - go1_ctrl_states_.default_joint_pos; // 5. (joint_pos - default_joint_pos)
    obDouble_.segment(24, 12) = go1_ctrl_states_.joint_vel; // 6. joint_vel
//    obDouble_.segment(36, 12) = go1_ctrl_states_.joint_actions; // 7. actions(clipped NN outputs)

    // scale the observation
    for (int i = 0; i < obDouble_.size(); i++) {
      obScaled_[i] = obDouble_[i] * scaleFactor_[i];
    }

    // clip the observation
    obScaled_ = obScaled_.cwiseMin(clipObs_).cwiseMax(-clipObs_);

    updateMovementMode();

  }


  void updateMovementMode() { // keep this: the movement mode in ctrl state is used in the EKF.
    // update joy cmd
    prev_joy_cmd_ctrl_state_ = joy_cmd_ctrl_state_;

    if (joy_cmd_ctrl_state_change_request_) {
      // toggle joy_cmd_ctrl_state
      joy_cmd_ctrl_state_ = joy_cmd_ctrl_state_ + 1;
      joy_cmd_ctrl_state_ = joy_cmd_ctrl_state_ % 2; //TODO: how to toggle more states?
      joy_cmd_ctrl_state_change_request_ = false; //erase this change request;
    }

    // determine movement mode
    if (joy_cmd_ctrl_state_ == 1) {
      // walking mode, in this mode the robot should execute gait
      go1_ctrl_states_.movement_mode = 1;
    } else if (joy_cmd_ctrl_state_ == 0 && prev_joy_cmd_ctrl_state_ == 1) {
      // leave walking mode
      go1_ctrl_states_.movement_mode = 0;
    } else {
      go1_ctrl_states_.movement_mode = 0;
    }

  } // updateMovementMode

  Eigen::VectorXd getObservation() { return obScaled_; }
  Go1CtrlStates getCtrlState() { return go1_ctrl_states_; }


  void joy_callback(const sensor_msgs::Joy::ConstPtr &joy_msg) { // This function applies for both gazebo and hardware
//  // left updown: change body height, not need now
//  joy_cmd_velz = joy_msg->axes[1] * JOY_CMD_BODY_HEIGHT_VEL;

    //A
    if (joy_msg->buttons[0] == 1) {
      joy_cmd_ctrl_state_change_request_ = true;
    }

    // right updown
    joy_cmd_velx_ = joy_msg->axes[4] * JOY_CMD_VELX_MAX;
    // right horiz
    joy_cmd_vely_ = joy_msg->axes[3] * JOY_CMD_VELY_MAX;
    // left horiz
    joy_cmd_yaw_rate_ = joy_msg->axes[0] * JOY_CMD_YAW_MAX;

    // lb
    if (joy_msg->buttons[4] == 1) {
      std::cout << "You have pressed the exit button!!!!" << std::endl;
      joy_cmd_exit_ = true;
    }
  }



 private:
  // 0,  1,  2: FL_hip, FL_thigh, FL_calf
  // 3,  4,  5: FR_hip, FR_thigh, FR_calf
  // 6,  7,  8: RL_hip, RL_thigh, RL_calf
  // 9, 10, 11: RR_hip, RR_thigh, RR_calf

  ros::Subscriber sub_joy_msg;

  // ! observation for RL controller
  size_t obDim_;

  Eigen::VectorXd obDouble_, obScaled_;
  Eigen::VectorXd actionMean_, actionStd_, obMean_, obStd_;

  Eigen::Vector3d linVelScale_, angVelScale_, gravityScale_, commandScale_;
  Eigen::VectorXd dofPosScale_, dofVelScale_;
  Eigen::VectorXd scaleFactor_;
  double clipObs_ = 100.;

  // joystick command
  double joy_cmd_velx_ = 0.0;
  double joy_cmd_vely_ = 0.0;
  double joy_cmd_velz_ = 0.0;

  double joy_cmd_yaw_rate_ = 0.0;

  double joy_cmd_body_height_ = 0.3;

  //  0 is standing, 1 is walking
  int joy_cmd_ctrl_state_ = 0;
  int prev_joy_cmd_ctrl_state_ = 0;
  bool joy_cmd_ctrl_state_change_request_ = false;
  bool joy_cmd_exit_ = false;

  Go1CtrlStates go1_ctrl_states_; // underscore to consistent with gazebo


  UNITREE_LEGGED_SDK::UDP udp;
  UNITREE_LEGGED_SDK::LowState state = {0};


};

