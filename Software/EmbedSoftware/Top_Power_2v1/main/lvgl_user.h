#ifndef _LVGL_USER_H_
#define _LVGL_USER_H_

/* includes -------------------------------------------------------------------*/
//lvgl
#include "lvgl.h"
//lvgl drivers
#include "lvgl_helpers.h"
//nxp guider
#include <gui_guider.h>
#include <events_init.h>
/* private includes -----------------------------------------------------------*/

/* exported types -------------------------------------------------------------*/

/* exported constants ---------------------------------------------------------*/

/* exported macro -------------------------------------------------------------*/

extern lv_indev_t * indev_keypad;
extern lv_indev_t * indev_encoder;
extern lv_ui guider_ui ;
extern SemaphoreHandle_t xGuiSemaphore ;


/* exported functions ---------------------------------------------------------*/
void guiTask(void *pvParameter);



#endif // !_LVGL_USER_H_