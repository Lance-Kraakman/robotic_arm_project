/*
 * trajectory.h
 *
 *  Created on: 20/11/2020
 *      Author: lance
 */
#include "../revoluteJoint/revoluteJoint.h"
#include <esp_log.h>
#include <vector>

#ifndef COMPONENTS_TRAJECTORY_TRAJECTORY_H_
#define COMPONENTS_TRAJECTORY_TRAJECTORY_H_

class trajectory {
public:
	trajectory();
	virtual ~trajectory();
	void set_joint(revoluteJoint joint_to_add);
	int set_step_array(std::vector<int> step_array, int periodSize);
	int set_period_array(std::vector<int> period_array, int arraySize);
	void setTrajectorySectionCount(int length);
	revoluteJoint get_joint();
	std::vector<int> get_step_array();
	std::vector<int> get_period_array();
	void operateStepperTrajectory(int currentTime); // (Different functions for stepper and servo)
	std::vector<int> step_array; // Number of time periods in corrosponding sections
	std::vector<int> period_array; // Time periods in corrosponding sections
	revoluteJoint joint; // Joints
	int trajectory_section_count;
	int step_count;
	int previous_time;
	int iterator;
};

#endif /* COMPONENTS_TRAJECTORY_TRAJECTORY_H_ */
