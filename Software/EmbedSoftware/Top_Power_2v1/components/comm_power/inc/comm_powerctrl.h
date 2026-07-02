
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

void comm_powerctrl_set_mode(uint8_t mode, uint16_t voltage, uint16_t current);
void comm_powerctrl_set_protect(uint16_t voltage, uint16_t current);
void comm_powerctrl_set_output(uint8_t output);
void comm_powerctrl_set_interval(uint8_t interval);
void comm_powerctrl_get_data( void );

#endif