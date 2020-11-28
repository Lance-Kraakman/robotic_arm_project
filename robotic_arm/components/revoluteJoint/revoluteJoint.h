/*
 * revoluteJoint.h
 *
 *  Created on: 18/11/2020
 *      Author: lance
 */
#include "../stepperDriver/stepperDriver.h"

#define STEP_ANGLE 1.8

#ifndef COMPONENTS_REVOLUTEJOINT_REVOLUTEJOINT_H_
#define COMPONENTS_REVOLUTEJOINT_REVOLUTEJOINT_H_

class revoluteJoint {
public:
	revoluteJoint(float cur_angle,float des_angle,float max_angle,float min_angle,float gear_ratio, float resolution, stepperDriver jointStepper);
	revoluteJoint();
	virtual ~revoluteJoint();
	stepperDriver jointStepper;
	float cur_angle;    		// current position (Theta)
	float des_angle;    		// desired position (Theta)
	float max_angle;  	    // min angle of joint rotation
	float min_angle;        // max angle of joint rotation
	float gear_ratio;       // output/input gear ratio
	float resolution;       // steper motor resolution
	float deg_per_step();   // Calculates efective degrees per step (of joint)
	float calc_steps_to(float angle);  // calculates steps to a desired position
	int check_range(float angle); //check to see if we can drive the stepper
	int joint_to_angle(float angle); // Moves the joint to the desired angle
	void update_position(float deg_per_step, int steps); // Caclulates the new position, taking into account discrete step sizes
};

#endif /* COMPONENTS_REVOLUTEJOINT_REVOLUTEJOINT_H_ */
