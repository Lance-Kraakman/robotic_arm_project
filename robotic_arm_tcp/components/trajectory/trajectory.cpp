/*
 * trajectory.cpp
 *
 *  Created on: 20/11/2020
 *      Author: lance
 */

#include "trajectory.h"
#include "esp_timer.h"

//static const char* TAG = "";

trajectory::trajectory() {
	// TODO Auto-generated constructor stub
	stepperDriver dummyDriver;
	revoluteJoint revJoin = revoluteJoint(0,0,0,0,0,0,dummyDriver);
	this->joint = revJoin;
	this->step_count = 0;
	this->previous_time = 0;
	this->step_array = {0}; // Number of time periods in corrosponding sections
	this->period_array = {0}; // Time periods in corrosponding sections (uS)
	this->dir_array = {0};
	this->trajectory_section_count = 0;
	this->iterator = 0;
}

trajectory::~trajectory() {
	// TODO Auto-generated constructor stub

}

void trajectory::set_joint(revoluteJoint joint_to_add) {
	this->joint = joint_to_add;
}

void trajectory::setTrajectorySectionCount(int length) {
	this->trajectory_section_count = length;
}

int trajectory::set_step_array(std::vector<int> step_array, int arraySize) {
	if (arraySize == this->trajectory_section_count) {
		this->step_array = step_array;
		return 1;
	} else {
		return -1;
	}
}

int trajectory::set_period_array(std::vector<int> period_array, int arraySize) {
	if (arraySize == this->trajectory_section_count) {
			this->period_array = period_array;
			return 1;
		} else {
			return -1;
		}
}

int trajectory::set_dir_array(std::vector<int> dir_array, int arraySize) {
	if (arraySize == this->trajectory_section_count) {
			this->dir_array = dir_array;
			return 1;
		} else {
			return -1;
		}
}

std::vector<int> trajectory::get_step_array() {
	return this->step_array;
}

std::vector<int> trajectory::get_period_array() {
	return this->period_array;
}

std::vector<int> trajectory::get_dir_array() {
	return this->dir_array;
}

stepperDriver trajectory::getStepperDriver() {
	return this->joint.jointStepper;
}

void trajectory::operateStepperTrajectory(int currentTime) {

	if (this->iterator < this->trajectory_section_count) { // check we still have trajectories/sections

		// Check if we need to do a step
		if ((currentTime - this->previous_time) >= (this->get_period_array()[this->iterator])) {
			this->getStepperDriver().set_stp(this->getStepperDriver().step_state);  //set the step high and low
			this->previous_time = currentTime;
			this->joint.jointStepper.step_state = !(this->joint.jointStepper.step_state);
			this->step_count++;
		}
		// Check if we have completed all of the steps in a "section"
		if (this->step_count >= abs(this->get_step_array()[this->iterator])) {
			this->step_count = 0; // Reset steo count
			this->iterator++;
			uint8_t direction = 0;
			direction = this->joint.jointStepper.getDirection(this->get_step_array()[this->iterator]); // get the direction of the next steps.
			this->joint.jointStepper.setDirection((unsigned char) direction);
		}
	}

}
