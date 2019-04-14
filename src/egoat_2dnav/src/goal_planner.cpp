#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

#define FIELD_LENGTH 15.24
#define NUM_PASSES_REQUIRED 38
//  Cutter head 100% coverage 0.4064 m

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  ros::init(argc, argv, "simple_navigation_goals");

  //  Tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  //  Wait for the action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }
  //  Create goal
  move_base_msgs::MoveBaseGoal goal;

  //  Referencing base_link (Robot relative position instead of map)
  goal.target_pose.header.frame_id = "base_link";

  //  Set Quaternion w value to 1.0
  goal.target_pose.pose.orientation.w = 1.0;

  //  i = columns, j = rows
  for(int i = 0; i < NUM_PASSES_REQUIRED; i++) {
    //  If even column, way point top of field
    if (i % 2 == 0) {
      goal.target_pose.pose.position.y = (FIELD_LENGTH - 0.15);
      ROS_INFO("Sending goal");
      goal.target_pose.header.stamp = ros::Time::now();
      ac.sendGoal(goal);
      ac.waitForResult();
      if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        ROS_INFO("Goal Successfully Reached");
      else
        ROS_INFO("Failed to reach goal");
    } else {
      //  0 position + .15 meter buffer
      goal.target_pose.pose.position.y = (0.15);
      ROS_INFO("Sending goal");
      goal.target_pose.header.stamp = ros::Time::now();
      ac.sendGoal(goal);
      ac.waitForResult();
      if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        ROS_INFO("Goal Successfully Reached");
      else
        ROS_INFO("Failed to reach goal");
    }
        //  Moving across field in X increasing order
        for (int j = 0; j < NUM_PASSES_REQUIRED; j++) {
          goal.target_pose.pose.position.x += (FIELD_LENGTH / NUM_PASSES_REQUIRED);
          ROS_INFO("Sending goal");
          goal.target_pose.header.stamp = ros::Time::now();
          ac.sendGoal(goal);
          ac.waitForResult();
          if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
            ROS_INFO("Goal Successfully Reached");
          else
            ROS_INFO("Failed to reach goal");
        }
  }
  return 0;
}
