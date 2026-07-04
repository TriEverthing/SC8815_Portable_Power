#ifndef _WIFI_WEBSERVER_H_
#define _WIFI_WEBSERVER_H_

/* includes -------------------------------------------------------------------*/
#include "stdio.h"
#include "stdint.h"
/* private includes -----------------------------------------------------------*/

/* exported types -------------------------------------------------------------*/

/* exported constants ---------------------------------------------------------*/

/* exported macro -------------------------------------------------------------*/
/* SoftAP Config */
#define WIFI_NVS_NAMESPACE  "wifi_cfg"
#define WIFI_NVS_SSID_KEY   "wifi_ssid"
#define WIFI_NVS_PSWD_KEY   "wifi_pswd"
#define SoftAP_SSID         "SC8815_Power" 
#define SoftAP_Password     "12345678"
#define SoftAP_Channel      6  


/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */

extern char current_ipv4[16];

/* exported functions ---------------------------------------------------------*/
void WiFi_Task(void *pvParmeters);

#endif // !_WIFI_WEBSERVER_H_