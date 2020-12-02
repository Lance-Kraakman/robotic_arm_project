/*
 * revoluteJoint.cpp
 *
 *  Created on: 18/11/2020
 *      Author: lance
 */

#include "revoluteJoint.h"
#include "math.h"

revoluteJoint::revoluteJoint(float cur_angle,float des_angle,float max_angle,float min_angle,float gear_ratio, float resolution,stepperDriver jointStepper) {
	// TODO Auto-generated constructor stub
	 this->cur_angle = cur_angle;       // current position (Theta)
	 this->des_angle = des_angle;       // desired position (Theta)
	 this->max_angle = max_angle;   // min angle of joint rotation
	 this->min_angle = min_angle;   // max angle of joint rotation
	 this->gear_ratio = gear_ratio; // output/input gear ratio
	 this->resolution = resolution; // steper motor resolution
	 this->jointStepper = jointStepper;
}

revoluteJoint::revoluteJoint() {
	 this->cur_angle = 0;       // current position (Theta)
		 this->des_angle = 0;       // desired position (Theta)
		 this->max_angle = 0;   // min angle of joint rotation
		 this->min_angle = 0;   // max angle of joint rotation
		 this->gear_ratio = 0; // output/input gear ratio
		 this->resolution = 0; // steper motor resolution
		 stepperDriver dummyStopper;
		 this->jointStepper = dummyStopper;
}

revoluteJoint::~revoluteJoint() {
	// Deconstructor
}

// Calculates the effective degree rotation of the joint per step of the stepper motor
float revoluteJoint::deg_per_step() {
	printf("Resolution: %f \n", this->resolution);
	printf("Ratio: %f \n", this->gear_ratio);
	return STEP_ANGLE * this->gear_ratio * this->resolution;
}

// Calculates the effective steps required to move the joint to the specified angle
float revoluteJoint::calc_steps_to(float angle) {
	printf("deg per step: %f \n", this->deg_per_step());
	return floor(this->cur_angle-angle)/this->deg_per_step();
}

// Checks wether we can rotate to the corrosponding angle
int revoluteJoint::check_range(float angle) {
	if ((angle <= this->max_angle) && (angle >= this->min_angle)) {
		return 1;
	} else {
		return 0;
	}
}

// Caclulate the new position (desired position may not be feasible due to discrete steps)
void revoluteJoint::update_position(float deg_per_step, int steps) {
	this->cur_angle = deg_per_step * steps;
}

// Rotates the joint to the desired angle. Returns -1 if it cannot do so
int revoluteJoint::joint_to_angle(float angle) {
	printf("ok 1");
	if (this->check_range(angle)) {
		printf("ok 2");
	uint8_t dir = 0;
	int steps_signed = calc_steps_to(angle); // The sign signals the direction to rotate
	int steps_unsigned = abs(steps_signed);
	printf("steps_signed: %d \n", steps_signed);
	dir = this->jointStepper.getDirection(steps_signed);
	/*if (steps_signed < 0) {
		dir = 1;
	}*/
		this->jointStepper.enable_stepper(); // Enable the stepper motor
		this->jointStepper.step_default(dir, steps_unsigned); // Drive the stepper
		this->jointStepper.disable_stepper(); // disable the stepper
		return 1;
	} else {
		return -1;
	}
}

