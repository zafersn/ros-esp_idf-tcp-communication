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
#include "esp_system.h"
#include "driver/gpio.h"
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

/**
 *
 *
 * // WiFi network name and password:
const char * networkName = "SUPERONLINE-WiFi_0668";
const char * networkPswd = "FTYFRMHNXYLU";
 *
 */
#define PORT CONFIG_EXAMPLE_PORT

#define BLINK_GPIO1 GPIO_NUM_23
#define BLINK_GPIO2 GPIO_NUM_22
#define BLINK_GPIO3 GPIO_NUM_21
#define BLINK_GPIO4 GPIO_NUM_19

static const char *TAG = "example";
volatile bool rx_complate = false;
volatile char rx_buffer[128];
volatile char tx_buff[128];

void blink_task(void *pvParameter)
{
    gpio_pad_select_gpio(BLINK_GPIO1);
    gpio_pad_select_gpio(BLINK_GPIO2);
    gpio_pad_select_gpio(BLINK_GPIO3);
    gpio_pad_select_gpio(BLINK_GPIO4);

    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO1, GPIO_MODE_OUTPUT);
    gpio_set_direction(BLINK_GPIO2, GPIO_MODE_OUTPUT);
    gpio_set_direction(BLINK_GPIO3, GPIO_MODE_OUTPUT);
    gpio_set_direction(BLINK_GPIO4, GPIO_MODE_OUTPUT);
	static bool a = false, b = false, c = false, d = false;
	ESP_LOGI(TAG, "LED TASK CREATE");
    while(1) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    	if(rx_complate)
    	{
//    		if((int)rx_buffer[0] != NULL)
//    		{
//    			printf("getData %c",rx_buffer[0]);
//    		}
    		switch((int)(rx_buffer[0])-'0')
    		{
				case 1:
					a = !a;
					gpio_set_level(BLINK_GPIO1,a );
					rx_buffer[0] = '5';
					strcpy(tx_buff,"TOP LED is ON");
					break;
				case 2:
					b = !b;
					gpio_set_level(BLINK_GPIO2,b );
					rx_buffer[0] = '5';
					strcpy(tx_buff,"BOTTOM LED is ON");
					break;
				case 3:
					c = !c;
					gpio_set_level(BLINK_GPIO3,c );
					rx_buffer[0] = '5';
					strcpy(tx_buff,"Right LED is ON");
					break;
				case 4:
					d = !d;
					gpio_set_level(BLINK_GPIO4,d );
					rx_buffer[0] = '5';
					strcpy(tx_buff,"LEFT LED is ON");
					break;
    		}

    	}
    }
}

static void tcp_server_task(void *pvParameters)
{
    char addr_str[128];
    int addr_family;
    int ip_protocol;



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
        if (listen_sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
          //  break;
        }
        ESP_LOGI(TAG, "Socket created");

        int opt = 1;
		int err = setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		if (err != 0)
		{
			ESP_LOGE(TAG, "Socket unable to set SO_REUSEADDR: errno %d", errno);
		//	break;
		}
         err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
           // break;
        }
        ESP_LOGI(TAG, "Socket bound, port %d", PORT);
        while (1) {
        err = listen(listen_sock, 1);
        if (err != 0) {
            ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
        uint addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket accepted");

        while (1) {
            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                rx_complate = false;
                break;
            }
            // Connection closed
            else if (len == 0) {
                ESP_LOGI(TAG, "Connection closed");
                rx_complate = false;
                break;
            }
            // Data received
            else {
                // Get the sender's ip address as string
                if (source_addr.sin6_family == PF_INET) {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
                } else if (source_addr.sin6_family == PF_INET6) {
                    inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
                }
                rx_complate = true;
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(TAG, "%s", rx_buffer);
                int err = send(sock, tx_buff, strlen(tx_buff), 0);
                if (err < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    break;
                }
            }
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            rx_complate = false;
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

static TaskHandle_t task_led_blink_manager = NULL;


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

    memset(rx_buffer,NULL,sizeof(rx_buffer));

    xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);

    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE*4, NULL, 5, &task_led_blink_manager);
}
