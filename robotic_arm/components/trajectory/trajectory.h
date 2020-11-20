/*
 * trajectory.h
 *
 *  Created on: 20/11/2020
 *      Author: lance
 */
#include "../revoluteJoint/revoluteJoint.h"

#ifndef COMPONENTS_TRAJECTORY_TRAJECTORY_H_
#define COMPONENTS_TRAJECTORY_TRAJECTORY_H_

class trajectory {
public:
	trajectory();
	int *recStepArray(); // pointer to array of array of steps
	int *recPeriodArray(); // pointer to an arrayof periods
	int **t_period_array;
	int **step_array;
	revoluteJoint *joints = NULL;
};

#endif /* COMPONENTS_TRAJECTORY_TRAJECTORY_H_ */
