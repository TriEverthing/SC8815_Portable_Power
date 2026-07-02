/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-07-07 10:14:07
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-07-07 22:15:53
 * @FilePath: \rda5820_fm_v2\components\RotaryEncoder\include\RotaryEncoder.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * RotaryEncoder.h
 *
 *  Created on: Jun 8, 2024
 *      Author: bluerose
 */

#ifndef USER_INC_ROTARYENCODER_H_
#define USER_INC_ROTARYENCODER_H_

#include <stdio.h>

extern uint16_t keypad_val ;
extern uint16_t encoder_val ;

void knob_init(void);
void button_init(void);

void knob_keypad_init(void);
int16_t get_knob_diffval(void);

#define get_keypad_val( x )     ( x = keypad_val , keypad_val = 0 )

#endif /* USER_INC_ROTARYENCODER_H_ */

