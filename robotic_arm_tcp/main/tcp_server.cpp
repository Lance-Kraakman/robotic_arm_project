/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "../components/stepperDriver/stepperDriver.h"
#include "../components/revoluteJoint/revoluteJoint.h"
#include "../components/trajectory/trajectory.h"


// C code for development is done in the windows linux subsystem on vs code

#define PORT CONFIG_EXAMPLE_PORT

static const char *TAG = "example";

typedef struct {
	int *steps;
	int *delays;
	int *dirs;
	int array_length;
	int path;
	int signal;

} trajectory_section;

#define TRAJ_SECTIONS 4
#define ARRAYS 3

QueueHandle_t trajectory_queue = NULL;


static void tcp_server_task(void *arg)
{

	trajectory_section sen_traj_sect;
	sen_traj_sect.signal = 1213;
    char rx_buffer[128]; // Buffer to send to socket
    sprintf(rx_buffer, "hello"); // hello check
    char addr_str[128];
    int addr_family;
    int ip_protocol;

    while (1) {
    	/* Standard setup for web socket from application example */
#ifdef CONFIG_EXAMPLE_IPV4
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
#else // IPV6
        struct sockaddr_in6 dest_addr;
        bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
        dest_addr.sin6_family = AF_INET6;
        dest_addr.sin6_port = htons(PORT);
        addr_family = AF_INET6;
        ip_protocol = IPPROTO_IPV6;
        inet6_ntoa_r(dest_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
#endif

        int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);

        int nodelay = 1;
		setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (void *)&nodelay, sizeof(int));

	   if (listen_sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket bound, port %d", PORT);

        err = listen(listen_sock, 1);
        if (err != 0) {
            ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
        uint addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        //int nodelay = 1;
		//setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (void *)&nodelay, sizeof(int));
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket accepted");
		setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (void *)&nodelay, sizeof(int));



        while (1) {
			int len = read(sock, rx_buffer, sizeof(rx_buffer) - 1);

			// Error occurred during receiving
			ESP_LOGI(TAG, "ok");
			if (len < 0) {
			   ESP_LOGE(TAG, "recv failed: errno %d", errno);
			   break;
			}

			// Connection closed
			else if (len == 0) {
			   ESP_LOGI(TAG, "Connection closed");
			   break;
			}

			//  Data received
			else {
			   // Get the sender's ip address as string
			   if (source_addr.sin6_family == PF_INET) {
				   inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
			   } else if (source_addr.sin6_family == PF_INET6) {
				   inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
			   }

			   rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
			   ESP_LOGI(TAG, "Received %d bytes", len);
			   ESP_LOGI(TAG, "%s", rx_buffer);


			   // If the queue is full -> Discard and dont add the trajectory but print the error
			   sen_traj_sect.signal = 9;


			   // TEST STRING
			   // step array : delay/period array : Dir Array : Path ID : array length : signal
			   // 3423,1234,86452,236745:9999,1234,86452,236745:8888,1234,86452,236745:0:4:0
			   int length = strlen(rx_buffer);
			   char str_cpy[length];
			   memset(str_cpy, '\0', sizeof(str_cpy));
			   strncpy(str_cpy, rx_buffer+1, (length-2));


			   printf("str cpy %s\n", str_cpy);
			   printf("Length %d\n First Element %c\n Last Element %c\n", length, rx_buffer[0], rx_buffer[length-1]);


			   // Check for starting and finishing signals of our buffer
			   if ((rx_buffer[0]=='*')&&(rx_buffer[length-1]=='#')) {

				   char *saveptr;
				   char *token;

				   char *saveptr2;
				   char *token2;

				   token = strtok_r(str_cpy, ":", &saveptr);

				   int *temp_array[ARRAYS]; // Array of pointers
				   int i = 0; int j = 0;

				   /* Allocate memory dynamically */
				   for(i=0;i<TRAJ_SECTIONS;i++) {
					   temp_array[i] = new int[TRAJ_SECTIONS]; //(void*) malloc(TRAJ_SECTIONS * sizeof(int));
				   }

				   i = 0;

				   while(token != NULL) {

					  // printf(" %s\n", token);
					   token2 = strtok_r(token, ",", &saveptr2);
					   j=0;
					   while(token2 !=NULL) {

						  // Put the recieved data into the trajectory structure
						   if (i<3) {
							   *(temp_array[i]+j) = atoi(token2);
						   } else if (i == 3) {
							   sen_traj_sect.path = atoi(token2);
						   } else if (i == 4) {
							   sen_traj_sect.array_length = atoi(token2);
						   } else if (i==5) {
							   sen_traj_sect.signal = atoi(token2);
						   }

						   token2 = strtok_r(NULL, ",", &saveptr2);
						   j++;
					   }
					   i++;
					   token = strtok_r(NULL, ":", &saveptr);
				   }

				   for(i=0;i<ARRAYS;i++) {
					   for(j=0;j<TRAJ_SECTIONS;j++) {
						   printf("%d: %d\n",i, *(temp_array[i]+j));
					   }
				   }

				   sen_traj_sect.steps = temp_array[0];
				   sen_traj_sect.delays = temp_array[1];
				   sen_traj_sect.dirs = temp_array[2];



				   if (xQueueSend(trajectory_queue, &sen_traj_sect, 0) == errQUEUE_FULL) {
					   ESP_LOGW(TAG, "QUEUE OVERFLOW. TO MANY TRAJECTORIES IN QUEU. DISCARDED");
				   }

			   }

			   int err = send(sock, rx_buffer, len, 0);

			   if (err < 0) {
				   ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
				   break;
			   }
			}

			vTaskDelay(50 / portTICK_PERIOD_MS);
			//taskYIELD();
		}

		if (sock != -1) {
			ESP_LOGE(TAG, "Shutting down socket and restarting...");
			shutdown(sock, 0);
			close(sock);
			fflush(stdout);
			esp_restart(); //lazy method restarts esp and therefore websocket. This should be changed
		}
	}
    vTaskDelete(NULL);
}



void runRoboticArm(void *arg) {
	//Initilize hardware pins for stepper driver
	stepperDriver trajOneDriver = stepperDriver(18,5,0,0,17);
	trajOneDriver.init_hardware(); // Initilizes the GPIO pins
	revoluteJoint trajOneJoint = revoluteJoint(0,0,50,-50,(1.0/48.0),1,trajOneDriver); // Initilize first stepper joint

	//Initilize hardware pins for stepper driver
	stepperDriver trajTwoDriver = stepperDriver(18,5,0,0,17);
	trajTwoDriver.init_hardware(); // Initilizes the GPIO pins
	revoluteJoint trajTwoJoint = revoluteJoint(0,0,50,-50,(1.0/48.0),1,trajTwoDriver); // Initilize first stepper joint

	//static const char* TAG = "MyModule";
	trajectory pathOne = trajectory();
	trajectory pathTwo = trajectory();

	// Add joints to Paths
	pathOne.joint = trajOneJoint;
	pathTwo.joint = trajTwoJoint;


	std::vector<trajectory> path_array; // Array of paths (One path for each motor)
	path_array.push_back(pathOne);
	path_array.push_back(pathTwo);

//	std::vector<int> step_values = {0, 1000, 2000, 3000, 4000};
//	std::vector<int> period_values = {0, 2500, 2500, 2500, 2500};
//
//	std::vector<int> step_values2 = {0, -3000, 2000, -3500, 4000};
//	std::vector<int> period_values2 = {0, 2500, 2500, 2500, 2500};




	int currentTime = esp_timer_get_time();

	//enter critical section
	//portENTER_CRITICAL(&mux);
	uint8_t data_rec = pdFALSE;
	trajectory_section rec_traj_sect;

	while(1) {


		data_rec = xQueueReceive(trajectory_queue, &rec_traj_sect, 0);

		if (data_rec == pdTRUE) { // We have an item in the queue
			ESP_LOGI(TAG, "array length %d", rec_traj_sect.array_length); // we have receieved all motor trajectories


			// Print out motor steps
			for(int j=0;j<rec_traj_sect.array_length;j++) {
				path_array[rec_traj_sect.path].step_array[j] = *(rec_traj_sect.steps+j);
				path_array[rec_traj_sect.path].period_array[j] = *(rec_traj_sect.delays+j);
				path_array[rec_traj_sect.path].dir_array[j] = *(rec_traj_sect.dirs+j);
				//printf("steps: %d, motor ID: %d, signal: %d \n",*(rec_traj_sect.steps+j),rec_traj_sect.path, rec_traj_sect.signal);
				printf("steps: %d, delays ID: %d, dirs: %d \n",*(rec_traj_sect.steps+j),*(rec_traj_sect.delays+j),*(rec_traj_sect.dirs+j));
			}

			// FREE ALLOCATED MEMORY BECAUSE WE DONT NEED IT ANYMORE! :) X
			//delete[] rec_traj_sect.steps;
		    //delete[] rec_traj_sect.delays;
			//delete[] rec_traj_sect.dirs;

		}

		// if signal equals one then we have collected the trojectories of all of the motors
		// And we should run the paths on the robotic arm
		if (rec_traj_sect.signal == 1) {

			currentTime = esp_timer_get_time();

			// Operate Stepper joitn Trajectories
			pathOne.operateStepperTrajectory(currentTime);
			pathTwo.operateStepperTrajectory(currentTime);

			//vTaskDelay(0); // 1 ticks to yield
			taskYIELD();
		}
		vTaskDelay(50/portTICK_PERIOD_MS);
	}
	//portEXIT_CRITICAL(&mux); //exit
}


extern "C" {
	void app_main();
}


void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());


    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    trajectory_queue = xQueueCreate(30, sizeof(trajectory_section));
    xTaskCreatePinnedToCore(tcp_server_task, "tcp_server", 8192, NULL, 25, NULL,0);
    xTaskCreatePinnedToCore(runRoboticArm, "runRoboticArm", 8192, NULL,25,NULL,1);

    //while(1) {
    //	 vTaskDelay(10 / portTICK_PERIOD_MS);
   // }
}
