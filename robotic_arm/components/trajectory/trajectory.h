/*
 * trajectory.h
 *
 *  Created on: 20/11/2020
 *      Author: lance
 */
#include "../revoluteJoint/revoluteJoint.h"
#include <vector>



#ifndef COMPONENTS_TRAJECTORY_TRAJECTORY_H_
#define COMPONENTS_TRAJECTORY_TRAJECTORY_H_

class trajectory {
public:
	trajectory();
	std::vector<std::vector<int>> step_arrays; // Number of time periods in corrosponding sections
	std::vector<std::vector<int>> period_arrays; // Time periods in corrosponding sections
	std::vector<revoluteJoint> joint_arrays; // Joints

};

#endif /* COMPONENTS_TRAJECTORY_TRAJECTORY_H_ */
