/*
 * trajectory.cpp
 *
 *  Created on: 20/11/2020
 *      Author: lance
 */

#include "trajectory.h"

trajectory::trajectory() {
	// TODO Auto-generated constructor stub
	this->step_count = 0;

}

trajectory::~trajectory() {
	// TODO Auto-generated constructor stub

}

void trajectory::set_joint(revoluteJoint joint_to_add) {
	this->joint = joint_to_add;
}

int trajectory::set_step_array(std::vector<int> step_array, int arraySize) {
	if (arraySize == this->trajectory_length) {
		this->step_array = step_array;
		return 1;
	} else {
		return -1;
	}
}

int trajectory::set_period_array(std::vector<int> period_array, int arraySize) {
	if (arraySize == this->trajectory_length) {
			this->period_array = period_array;
			return 1;
		} else {
			return -1;
		}
}

void trajectory::operateTrajectory(int currentTime) {

	// Check if we need to do a step
	if ((currentTime - this->previous_time) >= this->get_period_array()[this->period_array]) {
		this->joint.doStep(dir);
		this->previous_time = currentTime;
	}
	// Check if we have completed all of the steps in a "section"
	if (this->trajectory_length >= this->get_step_array()[this->step_count]) {
		this->step_count++;
	}
}
