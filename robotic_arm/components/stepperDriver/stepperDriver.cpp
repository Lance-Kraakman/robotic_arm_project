/*
 * motorDriver.cpp
 *
 *  Created on: 17/11/2020
 *      Author: lance
 */

#include "../stepperDriver/stepperDriver.h"
#include "driver/gpio.h"
#include "esp32/rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>

stepperDriver::stepperDriver(uint8_t stp,uint8_t dir,uint8_t MS1,uint8_t MS2,uint8_t EN) {
	this->stp = stp;
	this->dir = dir;
	this->MS1 = MS1;
	this->MS2 = MS2; //pull = step for tb6600
	this->EN = EN;
}

stepperDriver::stepperDriver() {
	// TODO Auto-generated destructor stub
	this->stp = 0;
	this->dir = 0;
	this->MS1 = 0;
	this->MS2 = 0; //pull = step for tb6600
	this->EN = 0;
}

stepperDriver::~stepperDriver() {
	// TODO Auto-generated destructor stub
}

// Inits Pins
void stepperDriver::init_hardware() {

	gpio_config_t io_conf; // configuration structure
	// Sets up the stepper driver gpio paramaters.
	io_conf.intr_type = (gpio_int_type_t) GPIO_PIN_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = (1ULL<<this->stp)|(1ULL<<this->dir)|(1ULL<<this->MS1)|
			(1ULL<<this->MS2)|(1ULL<<this->EN);
	io_conf.pull_down_en = (gpio_pulldown_t) GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en = (gpio_pullup_t) GPIO_PULLUP_DISABLE;
	gpio_config(&io_conf);

	vTaskDelay(20 / portTICK_RATE_MS);

	this->reset_stepper();

}

void stepperDriver::step_default(uint8_t direction, uint32_t steps) {
	gpio_set_level((gpio_num_t) this->dir, direction); //pull direction pin in correct direction

	for (int x = 0; x < steps; x++) {

		gpio_set_level((gpio_num_t) this->stp, HIGH);
		//vTaskDelay(10 / portTICK_RATE_MS);
		ets_delay_us(2500);//(5 / portTICK_RATE_MS);
		gpio_set_level((gpio_num_t) this->stp, LOW);
		//vTaskDelay(10 / portTICK_RATE_MS);
		ets_delay_us(2500);
		//vTaskDelay(5 / portTICK_RATE_MS);
	}
}

//Sets the microstepper mode if the stepper driver has MS1 and MS2
void stepperDriver::set_step_mode(uint8_t MS1, uint8_t MS2) {
	gpio_set_level((gpio_num_t) this->MS1, MS1);
	gpio_set_level((gpio_num_t) this->MS2, MS2);
}

void stepperDriver::reset_stepper() {
	gpio_set_level((gpio_num_t) this->stp, LOW);
	gpio_set_level((gpio_num_t) this->dir, LOW);
	gpio_set_level((gpio_num_t) this->EN, LOW);
}

//Print stepper information
void stepperDriver::stepper_info() {
	printf("STP %d, DIR %d, MS1 %d, MS2 %d, EN %d \n", this->stp, this->dir, this->MS1, this->MS2, this->EN);
}

void stepperDriver::enable_stepper() {
	gpio_set_level((gpio_num_t) this->EN, LOW);
}

void stepperDriver::disable_stepper() {
	gpio_set_level((gpio_num_t) this->EN, HIGH);
}
