#include <esp_log.h>
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "../components/stepperDriver/stepperDriver.h"
#include "../components/revoluteJoint/revoluteJoint.h"
#include "../components/trajectory/trajectory.h"

extern "C" {
	void app_main();
}

void app_main(void)
{
	 //Initilize hardware pins for stepper driver
	stepperDriver trajOneDriver = stepperDriver(18,5,0,0,17);
	trajOneDriver.init_hardware(); // Initilizes the GPIO pins

	// Define revolute joint
	revoluteJoint trajOneJoint = revoluteJoint(0,0,50,-50,(1.0/48.0),1,trajOneDriver);
	//trajOneJoint.joint_to_angle(-45); // Rotate 45 degrees

	//static const char* TAG = "MyModule";

	trajectory pathOne = trajectory();
	trajectory pathTwo = trajectory();

	std::vector<int> step_values = {0, 1000, 2000, 3000, 4000};
	std::vector<int> period_values = {0, 2500, 2500, 2500, 2500};

	std::vector<int> step_values2 = {0, -3000, 2000, -3500, 4000};
	std::vector<int> period_values2 = {0, 2500, 2500, 2500, 2500};

	pathOne.setTrajectorySectionCount(5);
	pathOne.set_period_array(period_values, 5);
	pathOne.set_step_array(step_values, 5);
	pathOne.joint = trajOneJoint;

	pathTwo.setTrajectorySectionCount(5);
	pathTwo.set_period_array(period_values2, 5);
	pathTwo.set_step_array(step_values2, 5);

	int currentTime = esp_timer_get_time();
	//pathOne.previous_time = currentTime;
//	int prevTime = currentTime;
	//ESP_LOGI(TAG, "time diff %d", (currentTime - prevTime));

	while(1) {
		currentTime = esp_timer_get_time();
		// Operate Stepper joitn Trajectories

		pathOne.operateStepperTrajectory(currentTime);
		pathTwo.operateStepperTrajectory(currentTime);

	//	prevTime = currentTime;



	}

}

