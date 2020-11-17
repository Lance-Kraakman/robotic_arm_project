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


extern "C" {
	void app_main();
}

class Greeting {
public:
	void helloEnglish() {
		::printf("Hello %s \n", name.c_str());
	}
	void helloFrench() {
		::printf("Bonjour %s \n", name.c_str());
	}
	void setName(std::string name) {
		this->name = name;
	}
private:
	std::string name = "";

};



void app_main(void)
{
	// Initilize hardware pins for stepper driver
	stepperDriver myDriver(26,25,0,0,33);
	myDriver.init_hardware(); // Initilizes the hardware
	myDriver.set_step_mode(LOW, LOW); // microstepper mode - See README
	myDriver.stepper_info();

	vTaskDelay(2500 / portTICK_RATE_MS);
	myDriver.enable_stepper();
	myDriver.step_default(LOW, 2000);
	vTaskDelay(3000 / portTICK_RATE_MS);
	myDriver.disable_stepper();


	while(1) {
		Greeting myGreeting;
		myGreeting.setName("Lance Kraakman");
		myGreeting.helloEnglish();
		myGreeting.helloFrench();
		vTaskDelay(5000/portTICK_RATE_MS);
	}
}

