#include "wifi_webserver.h"
//FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
//esp-idf drivers
#include "esp_system.h"
#include "esp_log.h"
#include "esp_timer.h"
//lwip
#include "lwip/err.h"
#include "lwip/sys.h"
/* ESP-IDF WiFi */
#include "esp_mac.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
//comm_power data include files
#include "power_data.h"
#include "comm_powerctrl.h"

//lvgl
#include "lvgl.h"
//nxp guider
#include <gui_guider.h>

extern lv_ui guider_ui ;
extern SemaphoreHandle_t xGuiSemaphore ;

char current_ipv4[16] = {0}; 
static const char *TAG = "WiFi";
static bool is_wifi_connected = false;


static void wifi_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) 
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "Station connected, MAC: " MACSTR ", AID=%d",MAC2STR(event->mac), event->aid);
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) 
    {
        ESP_LOGI(TAG, "Connect WiFi Successfully!");
        is_wifi_connected = true;
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Station Mode, Connect Router,IP:" IPSTR, IP2STR(&event->ip_info.ip));
        esp_ip4addr_ntoa(&event->ip_info.ip, current_ipv4, sizeof(current_ipv4));
        lv_label_set_text(guider_ui.ST7789V3_IPV4_Lable, current_ipv4);   
    }
}

extern const uint8_t _binary_index_html_start[] asm("_binary_index_html_start");
extern const uint8_t _binary_index_html_end[]   asm("_binary_index_html_end");

/* Handler for provisioning index page - return the WiFi config HTML to connected STA devices */
static esp_err_t softap_web_handler(httpd_req_t *req)
{
    /* Return simplified WiFi configuration page content */
    httpd_resp_set_type(req, "text/html");
    size_t html_len = _binary_index_html_end - _binary_index_html_start;
    return httpd_resp_send(req, (const char*)_binary_index_html_start, html_len);
}

/* Handler for WiFi credential submission - parse POST request, save SSID/password to NVS */
static esp_err_t connect_wifi_handler(httpd_req_t *req)
{
    char buf[256] = {0};
    /* Read POST body data from request */
    int ret = httpd_req_recv(req, buf, sizeof(buf)-1);
    if (ret <= 0) 
    {
        ESP_LOGE(TAG, "Failed to receive POST data");
        return ESP_FAIL;
    }

    /* Parse SSID and password from form data (simplified parsing) */
    char ssid[32] = {0}, pwd[64] = {0};
    sscanf(buf, "ssid=%31[^&]&pass=%63s", ssid, pwd);
    ESP_LOGI(TAG, "Received WiFi config - SSID: %s", ssid);
    //ESP_LOGI(TAG, "Received WiFi config - PassWord: %s", pwd);

    /* Save credentials to NVS and trigger WiFi station connection */
    nvs_handle_t nvs;
    ESP_ERROR_CHECK(nvs_open(WIFI_NVS_NAMESPACE, NVS_READWRITE, &nvs));
    ESP_ERROR_CHECK(nvs_set_str(nvs, WIFI_NVS_SSID_KEY, ssid));
    ESP_ERROR_CHECK(nvs_set_str(nvs, WIFI_NVS_PSWD_KEY, pwd));
    ESP_ERROR_CHECK(nvs_commit(nvs));
    nvs_close(nvs);

    /* Return success prompt to user browser */
    httpd_resp_sendstr(req, "Credentials saved, device will restart and connect to WiFi...");
    /* Restart system after short delay to apply new WiFi config */
    vTaskDelay(pdMS_TO_TICKS(2000));
    esp_restart();
    return ESP_OK;
}

void wifi_ap_webserver_start(void)
{
    // Create default network interfaces for AP modes
    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    esp_netif_ip_info_t ap_ip;
    esp_netif_get_ip_info(ap_netif, &ap_ip);
    esp_ip4addr_ntoa(&ap_ip.ip, current_ipv4, sizeof(current_ipv4));
    lv_label_set_text(guider_ui.ST7789V3_IPV4_Lable, current_ipv4);    

    /* Soft AP Mode */
    wifi_config_t ap_config = 
    {
        .ap = {
            .ssid = SoftAP_SSID,
            .ssid_len = strlen(SoftAP_SSID),
            .password = SoftAP_Password,
            .channel = SoftAP_Channel,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA2_PSK
        },
    };

    /* Close STA */
    ESP_ERROR_CHECK(esp_wifi_stop());
    /* Setting Wifi AP Mode */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    /* Improt Wifi Config */
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    /* Start Wifi */
    ESP_ERROR_CHECK(esp_wifi_start());

    /* Start the network configuration web service */
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    /* Satrt httpd sercer */
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    /* Register GET route for provisioning index page */
    static const httpd_uri_t index_route = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = softap_web_handler
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &index_route));

    /* Register POST route for WiFi credential submission */
    static const httpd_uri_t connect_route = {
        .uri = "/connect",
        .method = HTTP_POST,
        .handler = connect_wifi_handler
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &connect_route));
}


extern const uint8_t _binary_PowerServer_html_start[] asm("_binary_PowerServer_html_start");
extern const uint8_t _binary_PowerServer_html_end[]   asm("_binary_PowerServer_html_end");

/* Handler for provisioning index page - return the WiFi config HTML to connected STA devices */
static esp_err_t power_web_handler(httpd_req_t *req)
{
    /* Return simplified WiFi configuration page content */
    httpd_resp_set_type(req, "text/html");
    size_t html_len = _binary_PowerServer_html_end - _binary_PowerServer_html_start;
    return httpd_resp_send(req, (const char*)_binary_PowerServer_html_start, html_len);
}

/**
 * Unified handler to set both voltage and current parameters
 * Supports both URL GET query parameters and form POST request body
 */
esp_err_t power_web_set_handler(httpd_req_t *req)
{
    // Buffer for receiving POST data
    char post_buf[128] = {0};
    int recv_len = httpd_req_recv(req, post_buf, sizeof(post_buf)-1);
    
    // Return error if data reception fails
    if (recv_len <= 0) 
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Parameter reception failed");
        return ESP_FAIL;
    }

    // ---------------- power switch control logic  ----------------
    char *powerEnPtr = strstr(post_buf, "power_en=");
    int power_state = -1;
    if(powerEnPtr) 
    {
        sscanf(powerEnPtr + 9, "%d", &power_state);
        ESP_LOGI(TAG, "Power control command received, state: %d", power_state);
        // Call SC8815 power enable driver directly
        if( power_state )
        {
            power_data.set_switch_status = 1 ;
        }
        else
        {
            power_data.set_switch_status = 0 ;
              
        }
        comm_powerctrl_set_output(power_state);
    }

    // ---------------- voltage current setting logic  ----------------
    float targetVolt = 0.0f, targetCurr = 0.0f;
    // Parse 'volt' and 'current' parameters from request body
    char *voltPtr = strstr(post_buf, "volt=");
    char *currPtr = strstr(post_buf, "current=");

    if( voltPtr && currPtr )
    {
        if (voltPtr) sscanf(voltPtr + 5, "%f", &targetVolt);
        if (currPtr) sscanf(currPtr + 8, "%f", &targetCurr);

        // Send control commands to SC8815 hardware
        ESP_LOGE(TAG, "targetVolt:=%f V, targetCurr=%f A", targetVolt, targetCurr);
        power_data.set_voltage = (uint16_t)(targetVolt*1000+0.5); //mV
        power_data.set_current = (uint16_t)(targetCurr*1000+0.5); //mA
        comm_powerctrl_set_mode( 0 , power_data.set_voltage , power_data.set_current );
    }

    /* Update UI */
    if( xGuiSemaphore != NULL )
    {
        if ( xSemaphoreTake(xGuiSemaphore, portMAX_DELAY) == pdTRUE ) 
        {
            if( power_state >= 0 )
            {
                if(power_state ) 
                {
                    lv_obj_add_state(guider_ui.PowerSwitch, LV_STATE_CHECKED);
                    lv_label_set_text(guider_ui.ST7789V3_Status_Lable1, "ON"); 
                } 
                else 
                {
                    lv_obj_clear_state(guider_ui.PowerSwitch, LV_STATE_CHECKED);
                    lv_label_set_text(guider_ui.ST7789V3_Status_Lable1, "OFF"); 
                }
            }
            lv_spinbox_set_value(guider_ui.ST7789V3_SetVoltageSpinbox , power_data.set_voltage / 10 );
            lv_spinbox_set_value(guider_ui.ST7789V3_SetCurrentSpinbox , power_data.set_current / 10 );
            xSemaphoreGive(xGuiSemaphore);
        }
    }

    // Return success response
    httpd_resp_set_status(req, "200 OK");
    httpd_resp_sendstr(req, "Parameter setting successful");
    return ESP_OK;
}


static esp_err_t power_sample_data_handler(httpd_req_t *req)
{
    char resp_buf[256];
    float cur_volt = power_data.ina_read_voltage / 1000.0; 
    float cur_curr = power_data.ina_read_current / 1000.0; 
    float set_volt = power_data.set_voltage / 1000.0;
    float set_curr = power_data.set_current / 1000.0;
    uint8_t powerswitch = power_data.set_switch_status;
    int64_t timestamp = esp_timer_get_time() / 1000;

    snprintf(resp_buf, sizeof(resp_buf), 
             "{\"voltage\":%.3f,\"current\":%.3f,\"setvolt\":%.2f,\"setcurr\":%.2f,\"switch\":%d,\"timestamp\":%lld}",
             cur_volt, cur_curr, set_volt, set_curr, powerswitch, timestamp);

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, resp_buf, strlen(resp_buf));
    
    return ESP_OK;
}

// Manually read and validate WiFi information from custom NVS storage
bool wifi_sta_connect_saved_wifi(void)
{
    char ssid[33] = {0}, password[65] = {0};
    nvs_handle_t nvs_handle;
    esp_err_t err;
    /* Read credentials from NVS */ 
    err = nvs_open(WIFI_NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return false;
    }

    // First verify SSID key exists and has data
    size_t ssid_len = 32;
    err = nvs_get_str(nvs_handle, WIFI_NVS_SSID_KEY, ssid, &ssid_len);
    if (err != ESP_OK || ssid_len == 0) 
    {
        ESP_LOGW(TAG, "No valid SSID found in NVS");
        nvs_close(nvs_handle);
        return false;
    }

    // Read password
    size_t pwd_len = 65;
    err = nvs_get_str(nvs_handle, WIFI_NVS_PSWD_KEY, password, &pwd_len);
    if (err != ESP_OK) 
    {
        ESP_LOGW(TAG, "No WiFi password found in NVS");
        nvs_close(nvs_handle);
        return false;
    }

    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Custom NVS read successful, SSID: %s", ssid);
    
    // Create default network interfaces for STA modes
    esp_netif_create_default_wifi_sta();
    /* Setting Wifi Mode */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t sta_config = {0};
    strcpy((char*)sta_config.sta.ssid, ssid);
    strcpy((char*)sta_config.sta.password, password);

    /* Setting Wifi AP Mode */
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    /* Start Wifi */
    ESP_ERROR_CHECK(esp_wifi_start());
    /* Connect Wifi */
    ESP_ERROR_CHECK(esp_wifi_connect());

    /* Wait 5 seconds to verify connection success */ 
    vTaskDelay(pdMS_TO_TICKS(5000));

    if( is_wifi_connected )
    {
        /* Start the network configuration web service */
        httpd_handle_t server = NULL;
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.uri_match_fn = httpd_uri_match_wildcard;
        /* Satrt httpd sercer */
        ESP_ERROR_CHECK(httpd_start(&server, &config));

        /* Register GET route for provisioning index page */
        static const httpd_uri_t power_route = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = power_web_handler
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &power_route));

        /* Register POST route for WiFi credential submission */
        static const httpd_uri_t power_set_uri  = {
            .uri = "/api/power_set",
            .method = HTTP_POST,
            .handler = power_web_set_handler
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &power_set_uri ));  
        
        static const httpd_uri_t sample_uri  = {
            .uri = "/api/sample",
            .method = HTTP_GET,
            .handler = power_sample_data_handler
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &sample_uri )); 
    }

    return is_wifi_connected;
}


void WiFi_Task(void *pvParmeters)
{
    /* Initialize NVS */
    nvs_flash_init();
    /* Initialize the TCP/IP stack */
    ESP_ERROR_CHECK(esp_netif_init());
    /* Create an event loop, this is the brain of asynchronous communication */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    /* Event handler register */
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL));

    /* Initialize Wifi driver */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    if( wifi_sta_connect_saved_wifi() == false )
    {
        wifi_ap_webserver_start();
    }

    while(1)
    {

        //vTaskSuspend(NULL);
        vTaskDelete(NULL);
    }
}

