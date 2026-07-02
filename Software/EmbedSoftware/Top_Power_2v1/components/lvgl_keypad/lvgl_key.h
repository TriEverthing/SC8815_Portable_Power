#ifndef _LVGL_KEY_H_
#define _LVGL_KEY_H_

void lvgl_indev_driver_init(void);
void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

#endif // !_LVGL_KEY_H_
