
#include "esp_log.h"
#include <driver/gpio.h>
//lvgl
#include "lvgl.h"
//
#include "lvgl_key.h"


#define USE_ESP_IOT_ENCODER     1


#if USE_ESP_IOT_ENCODER
#include "rotaryencoder.h"
#endif

#define ENCODER_PA_PIN          GPIO_NUM_21
#define ENCODER_PB_PIN          GPIO_NUM_14
#define ENCODER_SW_PIN          GPIO_NUM_17

#define BUTTON_K0_PIN           GPIO_NUM_0
#define BUTTON_K1_PIN           GPIO_NUM_7
#define BUTTON_K2_PIN           GPIO_NUM_8
#define BUTTON_ACTIVE_LEVEL     0

static void keypad_indev_init(void);
static void knob_indev_init(void);

void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static uint8_t keypad_scan(void);

void knob_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
#if USE_ESP_IOT_ENCODER == 0
static void knob_isr_handler(void* arg);
#endif

static int32_t encoder_diff;
static lv_indev_state_t encoder_state;


void lvgl_indev_driver_init(void)
{
    keypad_indev_init();
    knob_indev_init();
}

/*Will be called by the library to read the mouse*/
void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint8_t last_key = 0;

    /*Get the current x and y coordinates*/
    //mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the a key is pressed and save the pressed key*/
    uint8_t act_key = keypad_scan();
    //ESP_LOGI("lvgl", "act_key=%d.",act_key);
    if(act_key != 0) 
    {
        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) 
        {
            case 1 << 0 :
                act_key = LV_KEY_NEXT;
                data->state = LV_INDEV_STATE_PR;
                //ESP_LOGI("lvgl", "LV_KEY_NEXT.");
                break;
            case 1 << 1 :
                act_key = LV_KEY_RIGHT;
                data->state = LV_INDEV_STATE_PR;
                //ESP_LOGI("lvgl", "LV_KEY_RIGHT.");
                break;
            case 1 << 2 :
                act_key = LV_KEY_LEFT;
                data->state = LV_INDEV_STATE_PR;
                //ESP_LOGI("lvgl", "LV_KEY_LEFT.");
                break;
            default :
                data->state = LV_INDEV_STATE_REL; //Press Up
            break;
        }

        last_key = act_key;
    }
    else 
    {
        data->state = LV_INDEV_STATE_REL;  //Press Up
    }

    data->key = last_key;
}


/*------------------
 * Encoder
 * -----------------*/

/*Will be called by the library to read the encoder*/
void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
#if USE_ESP_IOT_ENCODER
    data->enc_diff = -get_knob_diffval();
#else
    data->enc_diff = encoder_diff;
#endif
    data->state = encoder_state ;

    encoder_diff = 0 ;
}

static void keypad_indev_init(void)
{
    gpio_config_t key_cfg ;
    key_cfg.intr_type = GPIO_INTR_DISABLE ;
    key_cfg.mode = GPIO_MODE_INPUT ;
    key_cfg.pin_bit_mask = ( 1UL << BUTTON_K0_PIN ) | ( 1UL << BUTTON_K1_PIN ) | ( 1UL << BUTTON_K2_PIN )  ;
    key_cfg.pull_down_en = 0 ;
    key_cfg.pull_up_en = true ;
    gpio_config( &key_cfg );
}

static void knob_indev_init(void)
{
#if USE_ESP_IOT_ENCODER == 0
    gpio_config_t rotartyio_cfg = 
    {
        .pin_bit_mask = ( 1UL << ENCODER_PA_PIN ) ,
        .mode = GPIO_MODE_INPUT ,
        .pull_up_en = true ,
        .pull_down_en = 0 ,
        .intr_type = GPIO_INTR_ANYEDGE 
    };
    gpio_config( &rotartyio_cfg );

    gpio_install_isr_service(0);
    gpio_isr_handler_add( ENCODER_PA_PIN , knob_isr_handler  , NULL );

    rotartyio_cfg.intr_type = GPIO_INTR_DISABLE ;
    rotartyio_cfg.mode = GPIO_MODE_INPUT ;
    rotartyio_cfg.pin_bit_mask = ( 1UL << ENCODER_PB_PIN ) | ( 1UL << ENCODER_SW_PIN ) ;
    gpio_config( &rotartyio_cfg );
#endif  
    gpio_config_t gpio_cfg = 
    {
        .pin_bit_mask = ( 1UL << ENCODER_SW_PIN ) ,
        .mode = GPIO_MODE_INPUT ,
        .pull_up_en = true ,
        .pull_down_en = 0 ,
        .intr_type = GPIO_INTR_DISABLE 
    };
    gpio_config( &gpio_cfg );

#if USE_ESP_IOT_ENCODER
    knob_init();
#endif
}

#if USE_ESP_IOT_ENCODER == 0
static void knob_isr_handler(void* arg)
{
    static uint8_t knob_a_last_level = 0 , knob_b_last_level = 0 , knob_start_check = 0 ;
    uint8_t knob_a_act_level = 0 , knob_b_act_level = 0 ;

    //knob a rising or faling edges 
    if( knob_start_check == 0 )
    {
        knob_a_last_level = gpio_get_level( ENCODER_PA_PIN ) ;
        knob_b_last_level = gpio_get_level( ENCODER_PB_PIN ) ;
        knob_start_check = 1 ; 
    }
    else
    {
        knob_a_act_level = gpio_get_level( ENCODER_PA_PIN ) ;
        knob_b_act_level = gpio_get_level( ENCODER_PB_PIN ) ;

        if( (( knob_a_last_level == 0 && knob_a_act_level == 1 ) && ( knob_b_last_level == 1 && knob_b_act_level == 0 )) ||
            (( knob_a_last_level == 1 && knob_a_act_level == 0 ) && ( knob_b_last_level == 0 && knob_b_act_level == 1 )) )
        {        
            //konb a : 
            encoder_diff = encoder_diff - 1 ;
        }
        else if( (( knob_a_last_level == 0 && knob_a_act_level == 1 ) && ( knob_b_last_level == 0 && knob_b_act_level == 1 )) ||
                 (( knob_a_last_level == 1 && knob_a_act_level == 0 ) && ( knob_b_last_level == 1 && knob_b_act_level == 0 )) )
        {
            encoder_diff = encoder_diff + 1 ;
        }
        knob_a_last_level = 0xFF ;
        knob_b_last_level = 0xFF ;
        //encoder_diff = encoder_diff + knob_b_act_level - knob_b_last_level ;
        knob_start_check = 0 ;
    } 
}
#endif

static uint8_t keypad_scan(void)
{
    uint8_t act_key = 0xF8 ;
    act_key |= gpio_get_level( BUTTON_K0_PIN ) << 0 ;
    act_key |= gpio_get_level( BUTTON_K1_PIN ) << 1 ;
    act_key |= gpio_get_level( BUTTON_K2_PIN ) << 2 ;
    encoder_state = !gpio_get_level( ENCODER_SW_PIN ) ;
    act_key = ~act_key ;
    return act_key ;
}
