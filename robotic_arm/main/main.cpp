#include <esp_log.h>
#include <string>
#include <cstring>
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "../components/stepperDriver/stepperDriver.h"
#include "../components/revoluteJoint/revoluteJoint.h"

extern "C" {
	void app_main();
}

void app_main(void)
{
	// Initilize hardware pins for stepper driver
	stepperDriver myDriver = stepperDriver(26,25,0,0,33);
	myDriver.init_hardware(); // Initilizes the GPIO pins

	// Define revolute joint
	revoluteJoint robotic_joint = revoluteJoint(0,0,50,-50,(1.0/48.0),1,myDriver);
	robotic_joint.joint_to_angle(-45); // Rotate 45 degrees

	while(1) {

		//vTaskDelay(5000/portTICK_RATE_MS);
	}
}

