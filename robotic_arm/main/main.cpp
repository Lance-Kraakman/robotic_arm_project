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


static char tag[] = "cpp hello";

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
	motorDriver myDriver;
	while(1) {
		Greeting myGreeting;
		myGreeting.setName("Lance Kraakman");
		myGreeting.helloEnglish();
		myGreeting.helloFrench();
		vTaskDelay(5000/portTICK_RATE_MS);
	}
}

