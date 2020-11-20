/*
 * motorDriver.h
 *
 *  Created on: 17/11/2020
 *      Author: lance
 */

#include "esp_system.h"
#include "driver/gpio.h"

#define HIGH 1
#define LOW 0

#ifndef COMPONENTS_STEPPERDRIVER_STEPPERDRIVER_H_
#define COMPONENTS_STEPPERDRIVER_STEPPERDRIVER_H_

class stepperDriver {
public:
	// Dont declare params as private (as convention) because we will just take up code space
	// w/ getters and setters
	uint8_t stp;
	uint8_t dir;
	uint8_t MS1;
	uint8_t MS2;
	uint8_t EN;
	stepperDriver(uint8_t stp,uint8_t dir,uint8_t MS1,uint8_t MS2,uint8_t EN);
	stepperDriver();
	void init_hardware();
	void step_default(uint8_t direction, uint32_t steps);
	void set_step_mode(uint8_t MS1, uint8_t MS2);
	void reset_stepper();
	void enable_stepper();
	void disable_stepper();
	void single_step();
	void setDirection(uint8_t direction)
	void stepper_info();
	virtual ~stepperDriver();
};

#endif /* COMPONENTS_STEPPERDRIVER_STEPPERDRIVER_H_ */
