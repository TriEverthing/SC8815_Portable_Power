
#ifndef _COMM_POWERCTRL_H_
#define _COMM_POWERCTRL_H_

/* includes -------------------------------------------------------------------*/
#include "comm_frames.h"
/* private includes -----------------------------------------------------------*/

/* exported constants ---------------------------------------------------------*/

/* exported macro -------------------------------------------------------------*/
#define POWERCTRL_VC_SET        0x01
#define POWERCTRL_PORTECT       0x02
#define POWERCTRL_ON_OFF        0x03
#define POWERCTRL_DATA_RATE     0x04
#define POWERCTRL_READ_DATA     0x05
/* exported types -------------------------------------------------------------*/

/* exported variables ---------------------------------------------------------*/
extern comm_frames_handle_t power_frames ;
/* exported functions ---------------------------------------------------------*/
void comm_powerctrl_init(void);
void comm_powerctrl_Task(void *pvParameters);
void Power_Data_Task(void *pvParameters);
void comm_powerctrl_fan(uint8_t Fan_Newstate);

#endif