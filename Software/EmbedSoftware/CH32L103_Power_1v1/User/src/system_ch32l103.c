/********************************** (C) COPYRIGHT *******************************
 * File Name          : system_ch32l103.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/01/22
 * Description        : Device Peripheral Access Layer System Source File.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "ch32l103.h" 

/* 
* Uncomment the line corresponding to the desired System clock (SYSCLK) frequency (after 
* reset the HSI is used as SYSCLK source).
* If none of the define below is enabled, the HSI is used as System clock source. 
*/
//#define SYSCLK_FREQ_HSE    HSE_VALUE
//#define SYSCLK_FREQ_48MHz_HSE  48000000
//#define SYSCLK_FREQ_56MHz_HSE  56000000
//#define SYSCLK_FREQ_72MHz_HSE  72000000
//#define SYSCLK_FREQ_96MHz_HSE  96000000
//#define SYSCLK_FREQ_HSI    HSI_VALUE
//#define SYSCLK_FREQ_48MHz_HSI  48000000
//#define SYSCLK_FREQ_56MHz_HSI  56000000
//#define SYSCLK_FREQ_72MHz_HSI  72000000
#define SYSCLK_FREQ_96MHz_HSI  96000000
//#define SYSCLK_FREQ_HSI_LP   HSI_LP_VALUE                      /* Baud rate support less than 62.5Kbps when using UART */

/* Clock Definitions */
#ifdef SYSCLK_FREQ_HSE
uint32_t SystemCoreClock         = SYSCLK_FREQ_HSE;              /* System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_48MHz_HSE
uint32_t SystemCoreClock         = SYSCLK_FREQ_48MHz_HSE;        /* System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_56MHz_HSE
uint32_t SystemCoreClock         = SYSCLK_FREQ_56MHz_HSE;        /* System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_72MHz_HSE
uint32_t SystemCoreClock         = SYSCLK_FREQ_72MHz_HSE;        /* System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_96MHz_HSE
uint32_t SystemCoreClock         = SYSCLK_FREQ_96MHz_HSE;        /* System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_48MHz_HSI
uint32_t SystemCoreClock         = SYSCLK_FREQ_48MHz_HSI;        /* System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_56MHz_HSI
uint32_t SystemCoreClock         = SYSCLK_FREQ_56MHz_HSI;        /* System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_72MHz_HSI
uint32_t SystemCoreClock         = SYSCLK_FREQ_72MHz_HSI;        /* System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_96MHz_HSI
uint32_t SystemCoreClock         = SYSCLK_FREQ_96MHz_HSI;        /* System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_HSI_LP
uint32_t SystemCoreClock         = SYSCLK_FREQ_HSI_LP;           /* System Clock Frequency (Core Clock) */
#else
uint32_t SystemCoreClock         = HSI_VALUE;                    /* System Clock Frequency (Core Clock) */

#endif

__I uint8_t HBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};


/* system_private_function_proto_types */
static void SetSysClock(void);
static void System_GPIO_IPD_Unused(void);

#ifdef SYSCLK_FREQ_HSE
static void SetSysClockToHSE( void );
#elif defined SYSCLK_FREQ_48MHz_HSE
static void SetSysClockTo48_HSE( void );
#elif defined SYSCLK_FREQ_56MHz_HSE
static void SetSysClockTo56_HSE( void );
#elif defined SYSCLK_FREQ_72MHz_HSE
static void SetSysClockTo72_HSE( void );
#elif defined SYSCLK_FREQ_96MHz_HSE
static void SetSysClockTo96_HSE( void );
#elif defined SYSCLK_FREQ_48MHz_HSI
static void SetSysClockTo48_HSI( void );
#elif defined SYSCLK_FREQ_56MHz_HSI
static void SetSysClockTo56_HSI( void );
#elif defined SYSCLK_FREQ_72MHz_HSI
static void SetSysClockTo72_HSI( void );
#elif defined SYSCLK_FREQ_96MHz_HSI
static void SetSysClockTo96_HSI( void );
#elif defined SYSCLK_FREQ_HSI_LP
static void SetSysClockToHSI_LP( void );

#endif

/*********************************************************************
 * @fn      SystemInit
 *
 * @brief   Setup the microcontroller system Initialize the Embedded Flash Interface,
 *        the PLL and update the SystemCoreClock variable.
 *
 * @return  none
 */
void SystemInit (void)
{
  RCC->CTLR |= (uint32_t)0x00000001;
  RCC->CFGR0 &= (uint32_t)0x08FF0000;
  RCC->CTLR &= (uint32_t)0xFEF6FFFB;
  RCC->CTLR &= (uint32_t)0xFFFBFFFF;
  RCC->CFGR0 &= (uint32_t)0xFF00FFFF;
  RCC->INTR = 0x009F0000;
  SetSysClock();
}

/*********************************************************************
 * @fn      SystemCoreClockUpdate
 *
 * @brief   Update SystemCoreClock variable according to Clock Register Values.
 *
 * @return  none
 */
void SystemCoreClockUpdate (void)
{
  uint32_t tmp = 0, pllmull = 0, pllsource = 0;

  tmp = RCC->CFGR0 & RCC_SWS;

  switch (tmp)
  {
    case 0x00:
        if(RCC->CTLR & (1<<2))
        {
            SystemCoreClock = HSI_LP_VALUE;
        }
        else {
            SystemCoreClock = HSI_VALUE;
        }
      break;
    case 0x04:
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x08:
      pllmull = RCC->CFGR0 & RCC_PLLMULL;
      pllsource = RCC->CFGR0 & RCC_PLLSRC;
      pllmull = ( pllmull >> 18) + 2;

      if(pllmull == 17) pllmull = 18;

      if (pllsource == 0x00)
      {
          if(EXTEN->EXTEN_CTR & EXTEN_PLL_HSI_PRE){
              SystemCoreClock = HSI_VALUE * pllmull;
          }
          else{
              SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
          }
      }
      else
      {
          if ((RCC->CFGR0 & RCC_PLLXTPRE) != (uint32_t)RESET)
          {
              SystemCoreClock = (HSE_VALUE >> 1) * pllmull;
          }
          else
          {
              SystemCoreClock = HSE_VALUE * pllmull;
          }
      }

      break;
    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }

  tmp = HBPrescTable[((RCC->CFGR0 & RCC_HPRE) >> 4)];
  SystemCoreClock >>= tmp;
}

/*********************************************************************
 * @fn      SetSysClock
 *
 * @brief   Configures the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClock(void)
{
  System_GPIO_IPD_Unused();

#ifdef SYSCLK_FREQ_HSE
    SetSysClockToHSE();
#elif defined SYSCLK_FREQ_48MHz_HSE
    SetSysClockTo48_HSE();
#elif defined SYSCLK_FREQ_56MHz_HSE
    SetSysClockTo56_HSE();
#elif defined SYSCLK_FREQ_72MHz_HSE
    SetSysClockTo72_HSE();
#elif defined SYSCLK_FREQ_96MHz_HSE
    SetSysClockTo96_HSE();
#elif defined SYSCLK_FREQ_48MHz_HSI
    SetSysClockTo48_HSI();
#elif defined SYSCLK_FREQ_56MHz_HSI
    SetSysClockTo56_HSI();
#elif defined SYSCLK_FREQ_72MHz_HSI
    SetSysClockTo72_HSI();
#elif defined SYSCLK_FREQ_96MHz_HSI
    SetSysClockTo96_HSI();
#elif defined SYSCLK_FREQ_HSI_LP
    SetSysClockToHSI_LP();

#endif
 
 /* If none of the define above is enabled, the HSI is used as System clock
  * source (default after reset) 
  */
}


#ifdef SYSCLK_FREQ_HSE

/*********************************************************************
 * @fn      SetSysClockToHSE
 *
 * @brief   Sets HSE as System clock source and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockToHSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* Flash 0 wait state */
  FLASH->ACTLR = (uint32_t)FLASH_ACTLR_LATENCY_0;
   
  RCC->CTLR |= ((uint32_t)RCC_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* HCLK = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;      
    /* PCLK2 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
    /* PCLK1 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV1;
    
    /* Select HSE as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_HSE;    

    /* Wait till HSE is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x04)
    {
    }
  }
  else
  { 
    /* Flash 0 wait state */
    FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);

    /* If HSE fails to start-up, the application will have wrong clock
     * configuration. User can add here some code to deal with this error 
     */
  }  
}

#elif defined SYSCLK_FREQ_48MHz_HSE

/*********************************************************************
 * @fn      SetSysClockTo48_HSE
 *
 * @brief   Sets System clock frequency to 48MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo48_HSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  /* Flash 1 wait state */
  FLASH->ACTLR = (uint32_t)FLASH_ACTLR_LATENCY_1;
   
  RCC->CTLR |= ((uint32_t)RCC_HSEON);
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* HCLK = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;    
    /* PCLK2 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;  
    /* PCLK1 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;

    /* PLL configuration: PLLCLK = HSE * 6 = 48 MHz (HSE=8MHZ) */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE | RCC_PLLMULL));
    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL6);

    /* Enable PLL */
    RCC->CTLR |= RCC_PLLON;
    /* Wait till PLL is ready */
    while((RCC->CTLR & RCC_PLLRDY) == 0)
    {
    }
    /* Select PLL as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;    
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { 
    /* Flash 0 wait state */
    FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);

    /* If HSE fails to start-up, the application will have wrong clock
     * configuration. User can add here some code to deal with this error
     */
  } 
}

#elif defined SYSCLK_FREQ_56MHz_HSE

/*********************************************************************
 * @fn      SetSysClockTo56_HSE
 *
 * @brief   Sets System clock frequency to 56MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo56_HSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  /* Flash 1 wait state */
  FLASH->ACTLR = (uint32_t)FLASH_ACTLR_LATENCY_1;

  RCC->CTLR |= ((uint32_t)RCC_HSEON);

  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* HCLK = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;   
    /* PCLK2 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
    /* PCLK1 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;
  
    /* PLL configuration: PLLCLK = HSE * 7 = 56 MHz (HSE=8MHZ) */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE | RCC_PLLMULL));
    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL7);

    /* Enable PLL */
    RCC->CTLR |= RCC_PLLON;
    /* Wait till PLL is ready */
    while((RCC->CTLR & RCC_PLLRDY) == 0)
    {
    }

    /* Select PLL as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;    
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  {
    /* Flash 0 wait state */
    FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);

    /* If HSE fails to start-up, the application will have wrong clock
    * configuration. User can add here some code to deal with this error
    */
  } 
}

#elif defined SYSCLK_FREQ_72MHz_HSE

/*********************************************************************
 * @fn      SetSysClockTo72_HSE
 *
 * @brief   Sets System clock frequency to 72MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo72_HSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
     
  /* Flash 1 wait state */
  FLASH->ACTLR = (uint32_t)FLASH_ACTLR_LATENCY_1;

  RCC->CTLR |= ((uint32_t)RCC_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* HCLK = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1; 
    /* PCLK2 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1; 
    /* PCLK1 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;
 
    /* PLL configuration: PLLCLK = HSE * 9 = 72 MHz (HSE=8MHZ) */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE | RCC_PLLMULL));
    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL9);

    /* Enable PLL */
    RCC->CTLR |= RCC_PLLON;
    /* Wait till PLL is ready */
    while((RCC->CTLR & RCC_PLLRDY) == 0)
    {
    }    
    /* Select PLL as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;    
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { 
    /* Flash 0 wait state */
    FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);

    /* If HSE fails to start-up, the application will have wrong clock
    * configuration. User can add here some code to deal with this error
    */
  }
}


#elif defined SYSCLK_FREQ_96MHz_HSE

/*********************************************************************
 * @fn      SetSysClockTo96_HSE
 *
 * @brief   Sets System clock frequency to 96MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo96_HSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  /* Flash 2 wait state */
  FLASH->ACTLR = (uint32_t)FLASH_ACTLR_LATENCY_2;

  RCC->CTLR |= ((uint32_t)RCC_HSEON);

  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }

  if (HSEStatus == (uint32_t)0x01)
  {
    /* HCLK = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
    /* PCLK2 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
    /* PCLK1 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;

    /* PLL configuration: PLLCLK = HSE * 12 = 96 MHz (HSE=8MHZ) */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE | RCC_PLLMULL));
    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL12);

    /* Enable PLL */
    RCC->CTLR |= RCC_PLLON;
    /* Wait till PLL is ready */
    while((RCC->CTLR & RCC_PLLRDY) == 0)
    {
    }
    /* Select PLL as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  {
    /* Flash 0 wait state */
    FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);

    /* If HSE fails to start-up, the application will have wrong clock
    * configuration. User can add here some code to deal with this error
    */
  }
}

#elif defined SYSCLK_FREQ_48MHz_HSI

/*********************************************************************
 * @fn      SetSysClockTo48_HSI
 *
 * @brief   Sets System clock frequency to 48MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo48_HSI(void)
{
    /* Flash 1 wait state */
    FLASH->ACTLR = (uint32_t)FLASH_ACTLR_LATENCY_1;

    EXTEN->EXTEN_CTR |= EXTEN_PLL_HSI_PRE;

    /* HCLK = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
    /* PCLK2 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
    /* PCLK1 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;

    /*  PLL configuration: PLLCLK = HSI * 6 = 48 MHz */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE | RCC_PLLMULL));
    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSI_Div2 | RCC_PLLMULL6);

    /* Enable PLL */
    RCC->CTLR |= RCC_PLLON;
    /* Wait till PLL is ready */
    while((RCC->CTLR & RCC_PLLRDY) == 0)
    {
    }
    /* Select PLL as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
}

#elif defined SYSCLK_FREQ_56MHz_HSI

/*********************************************************************
 * @fn      SetSysClockTo56_HSI
 *
 * @brief   Sets System clock frequency to 56MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo56_HSI(void)
{
    /* Flash 1 wait state */
    FLASH->ACTLR = (uint32_t)FLASH_ACTLR_LATENCY_1;

    EXTEN->EXTEN_CTR |= EXTEN_PLL_HSI_PRE;

    /* HCLK = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
    /* PCLK2 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
    /* PCLK1 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;

     /* PLL configuration: PLLCLK = HSI * 7 = 56 MHz */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE | RCC_PLLMULL));
    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSI_Div2 | RCC_PLLMULL7);

    /* Enable PLL */
    RCC->CTLR |= RCC_PLLON;
    /* Wait till PLL is ready */
    while((RCC->CTLR & RCC_PLLRDY) == 0)
    {
    }
    /* Select PLL as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
}

#elif defined SYSCLK_FREQ_72MHz_HSI

/*********************************************************************
 * @fn      SetSysClockTo72_HSI
 *
 * @brief   Sets System clock frequency to 72MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo72_HSI(void)
{
    /* Flash 1 wait state */
    FLASH->ACTLR = (uint32_t)FLASH_ACTLR_LATENCY_1;

    EXTEN->EXTEN_CTR |= EXTEN_PLL_HSI_PRE;

    /* HCLK = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
    /* PCLK2 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
    /* PCLK1 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;

    /*  PLL configuration: PLLCLK = HSI * 9 = 72 MHz */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE | RCC_PLLMULL));
    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSI_Div2 | RCC_PLLMULL9);

    /* Enable PLL */
    RCC->CTLR |= RCC_PLLON;
    /* Wait till PLL is ready */
    while((RCC->CTLR & RCC_PLLRDY) == 0)
    {
    }
    /* Select PLL as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
}

#elif defined SYSCLK_FREQ_96MHz_HSI

/*********************************************************************
 * @fn      SetSysClockTo96_HSI
 *
 * @brief   Sets System clock frequency to 96MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo96_HSI(void)
{
    /* Flash 2 wait state */
    FLASH->ACTLR = (uint32_t)FLASH_ACTLR_LATENCY_2;

    EXTEN->EXTEN_CTR |= EXTEN_PLL_HSI_PRE;

    /* HCLK = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
    /* PCLK2 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
    /* PCLK1 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;

    /* PLL configuration: PLLCLK = HSI * 12 = 96 MHz */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE | RCC_PLLMULL));
    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSI_Div2 | RCC_PLLMULL12);

    /* Enable PLL */
    RCC->CTLR |= RCC_PLLON;
    /* Wait till PLL is ready */
    while((RCC->CTLR & RCC_PLLRDY) == 0)
    {
    }
    /* Select PLL as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
}

#elif defined SYSCLK_FREQ_HSI_LP

/*********************************************************************
 * @fn      SetSysClockToHSI_LP
 *
 * @brief   Sets HSI as System clock source of Low power and configure HCLK, PCLK2 and PCLK1 prescalers.
 *          In this mode, HSI is 1MHz and Baud rate support less than 62.5Kbps when using UART.
 *
 * @return  none
 */
static void SetSysClockToHSI_LP(void)
{
    uint32_t tmpreg = 0;

    RCC->CTLR |= RCC_HSILP ;

    /* HCLK = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
    /* PCLK2 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
    /* PCLK1 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV1;
    /* Low power mode HSI  */
    tmpreg = RCC->CTLR;
    tmpreg &= ~RCC_HSITRIM ;
    tmpreg |= (uint32_t)((*(uint8_t*)HSI_LP_TRIM_BASE)&0x1F) << 3;
    RCC->CTLR = tmpreg;
}

#endif

/*********************************************************************
 * @fn      GPIO_IPD_Unused
 *
 * @brief   Configure unused GPIO as input pull-down.
 *
 * @param   none
 *
 * @return  none
 */
void System_GPIO_IPD_Unused(void)
{
    uint32_t chip = 0;
    // uint32_t OPA_Trim = 0;
    // uint16_t ADC_Trim = 0;
    // uint32_t TS_Val = 0;
    // uint32_t CHIPID = 0;
    // uint16_t USBPD_CFG = 0;

    // OPA_Trim = (((*(uint32_t *)OPA_TRIM_BASE & 0x0000001F) << 25) | (((*(uint32_t *)OPA_TRIM_BASE & 0x00008000)^0x00008000) << 9)) \
    //         | (((*(uint32_t *)OPA_TRIM_BASE & 0x001F0000) << 1) | (((*(uint32_t *)OPA_TRIM_BASE & 0x80000000)^0x80000000) >> 15));
    // ADC_Trim = (*(uint16_t *)ADC_TRIM_BASE);
    // TS_Val = (*(uint32_t *)TS_BASE);
    // CHIPID = (*(uint32_t *)CHIPID_BASE);
    // USBPD_CFG = (*(uint16_t *)USBPD_CFG_BASE);

    RCC->PB2PCENR |= RCC_IOPAEN |RCC_IOPBEN | RCC_IOPCEN | RCC_IOPDEN | RCC_AFIOEN ;
    chip =  *( uint32_t * )CHIPID_BASE & (~0x000000F0);
    switch(chip)
    {
        case 0x10320700:     //CH32L103K8U6
        {
            //PB10,PB11,PB12,PB13,PB14
            GPIOB->CFGHR = ( GPIOB->CFGHR  & ( 
              ~( GPIO_CFGHR_MODE10 | GPIO_CFGHR_MODE11 | GPIO_CFGHR_MODE12 | GPIO_CFGHR_MODE13 | GPIO_CFGHR_MODE14 
               | GPIO_CFGHR_CNF10 | GPIO_CFGHR_CNF11 | GPIO_CFGHR_CNF12 | GPIO_CFGHR_CNF13 | GPIO_CFGHR_CNF14 ) ) )  
             | ( GPIO_CFGHR_CNF10_1 | GPIO_CFGHR_CNF11_1 | GPIO_CFGHR_CNF12_1 | GPIO_CFGHR_CNF13_1 | GPIO_CFGHR_CNF14_1 ) ;
            GPIOB->BSHR = ( (uint16_t)0x0001 << 10 ) | ( (uint16_t)0x0001 << 11 ) | ( (uint16_t)0x0001 << 12 ) | ( (uint16_t)0x0001 << 13 ) | ( (uint16_t)0x0001 << 14 ) ;       
            //PC13
            GPIOC->CFGHR = ( GPIOC->CFGHR  & ( ~( GPIO_CFGHR_MODE13 | GPIO_CFGHR_CNF13 ) ) )   | ( GPIO_CFGHR_CNF13_1 ) ;
            GPIOC->BSHR = ( (uint16_t)0x0001 << 13 ) ;
            break;
        }
        case 0x103D0700:     //CH32L103F8U6
        {
            AFIO->PCFR1 |= AFIO_PCFR1_PD01_RM ;
            //PA15
            GPIOA->CFGHR = ( GPIOA->CFGHR  & ( ~( GPIO_CFGHR_MODE15 | GPIO_CFGHR_CNF15 ) ) ) | ( GPIO_CFGHR_CNF15_1 ) ;
            GPIOA->BSHR = ( (uint16_t)0x0001 << 15 ) ;
            //PB3,PB4,PB5,PB8,PB9,PB12
            GPIOB->CFGLR = ( GPIOB->CFGLR  & ( 
              ~( GPIO_CFGLR_MODE3 | GPIO_CFGLR_MODE4 | GPIO_CFGLR_MODE5 | GPIO_CFGLR_CNF3 | GPIO_CFGLR_CNF4 | GPIO_CFGLR_CNF5 ) ) )  
             | ( GPIO_CFGLR_CNF3_1 | GPIO_CFGLR_CNF4_1 | GPIO_CFGLR_CNF5_1  ) ;
            GPIOB->CFGHR = ( GPIOB->CFGHR  & ( 
              ~( GPIO_CFGHR_MODE8 | GPIO_CFGHR_MODE9 | GPIO_CFGHR_MODE12 | GPIO_CFGHR_CNF8 | GPIO_CFGHR_CNF9 | GPIO_CFGHR_CNF12 ) ) )  
             | ( GPIO_CFGHR_CNF8_1 | GPIO_CFGHR_CNF9_1 | GPIO_CFGHR_CNF12_1  ) ;
            GPIOB->BSHR = ( (uint16_t)0x0001 << 3 ) | ( (uint16_t)0x0001 << 4 ) | ( (uint16_t)0x0001 << 5 ) | ( (uint16_t)0x0001 << 8 ) 
                         | ( (uint16_t)0x0001 << 9 ) | ( (uint16_t)0x0001 << 12 ) ;
			      //PB2
            GPIOB->CFGLR = ( GPIOB->CFGLR  & ( ~( GPIO_CFGLR_MODE2 | GPIO_CFGLR_CNF2 ) ) ) | ( GPIO_CFGHR_CNF15_1 ) ;
            GPIOB->BCR = ( (uint16_t)0x0001 << 2 ) ;
            //PC13,PC14,PC15
            GPIOC->CFGHR = ( GPIOB->CFGHR  & ( 
              ~( GPIO_CFGHR_MODE13 | GPIO_CFGHR_MODE14 | GPIO_CFGHR_MODE15 | GPIO_CFGHR_CNF13 | GPIO_CFGHR_CNF14 | GPIO_CFGHR_CNF15 ) ) )  
             | ( GPIO_CFGHR_CNF13_1 | GPIO_CFGHR_CNF14_1 | GPIO_CFGHR_CNF15_1  ) ;
            GPIOC->BSHR = ( (uint16_t)0x0001 << 13 ) | ( (uint16_t)0x0001 << 14 ) | ( (uint16_t)0x0001 << 15 );
            //PD0,PD1
            GPIOB->CFGLR = ( GPIOB->CFGLR  & ( 
              ~( GPIO_CFGLR_MODE0 | GPIO_CFGLR_MODE1 | GPIO_CFGLR_CNF0 | GPIO_CFGLR_CNF1 ) ) )  
             | ( GPIO_CFGLR_CNF0_1 | GPIO_CFGLR_CNF1_1  ) ;
            GPIOD->BSHR = ( (uint16_t)0x0001 << 0 ) | ( (uint16_t)0x0001 << 1 ) ;
            break;
        }
        case 0x103B0700:     //CH32L103G8R6
        {
            AFIO->PCFR1 |= AFIO_PCFR1_PD01_RM ;
            //PA15
            GPIOA->CFGHR = ( GPIOA->CFGHR  & ( ~( GPIO_CFGHR_MODE15 | GPIO_CFGHR_CNF15 ) ) ) | ( GPIO_CFGHR_CNF15_1 ) ;
            GPIOA->BSHR = ( (uint16_t)0x0001 << 15 ) ;
            //PB4,PB9
            GPIOB->CFGLR = ( GPIOB->CFGLR  & ( ~( GPIO_CFGLR_MODE4 | GPIO_CFGLR_CNF4 ) ) ) | ( GPIO_CFGLR_CNF4_1 ) ;
            GPIOB->CFGHR = ( GPIOB->CFGHR  & ( ~( GPIO_CFGHR_MODE9 | GPIO_CFGHR_CNF9 ) ) ) | ( GPIO_CFGHR_CNF9_1 ) ;
            GPIOB->BSHR = ( (uint16_t)0x0001 << 4 ) | ( (uint16_t)0x0001 << 9 ) ;
			      //PB2
            GPIOB->CFGLR = ( GPIOB->CFGLR  & ( ~( GPIO_CFGLR_MODE2 | GPIO_CFGLR_CNF2 ) ) ) | ( GPIO_CFGHR_CNF15_1 ) ;
            GPIOB->BCR = ( (uint16_t)0x0001 << 2 ) ;
            //PC13,PC14,PC15
            GPIOC->CFGHR = ( GPIOB->CFGHR  & ( 
              ~( GPIO_CFGHR_MODE13 | GPIO_CFGHR_MODE14 | GPIO_CFGHR_MODE15 | GPIO_CFGHR_CNF13 | GPIO_CFGHR_CNF14 | GPIO_CFGHR_CNF15 ) ) )  
             | ( GPIO_CFGHR_CNF13_1 | GPIO_CFGHR_CNF14_1 | GPIO_CFGHR_CNF15_1  ) ;
            GPIOC->BSHR = ( (uint16_t)0x0001 << 13 ) | ( (uint16_t)0x0001 << 14 ) | ( (uint16_t)0x0001 << 15 );
            //PD1
            GPIOB->CFGLR = ( GPIOB->CFGLR  & ( ~(  GPIO_CFGLR_MODE1 | GPIO_CFGLR_CNF1 ) ) ) | ( GPIO_CFGLR_CNF1_1  ) ;
            GPIOD->BSHR = ( (uint16_t)0x0001 << 1 ) ;
            break;
        }
        case 0x103A0700:     //CH32L103F8P6
        {
            //PA8,P9,PA10,PA15
            GPIOA->CFGHR = ( GPIOA->CFGHR  & ( 
              ~( GPIO_CFGHR_MODE8 | GPIO_CFGHR_MODE9 | GPIO_CFGHR_MODE10 | GPIO_CFGHR_MODE15 
               | GPIO_CFGHR_CNF8 | GPIO_CFGHR_CNF9 | GPIO_CFGHR_CNF10 | GPIO_CFGHR_CNF15 ) ) )  
             | ( GPIO_CFGHR_CNF8_1 | GPIO_CFGHR_CNF9_1 | GPIO_CFGHR_CNF10_1 | GPIO_CFGHR_CNF15_1) ;
            GPIOA->BSHR = ( (uint16_t)0x0001 << 8) | ( (uint16_t)0x0001 << 9 ) | ( (uint16_t)0x0001 << 10 ) | ( (uint16_t)0x0001 << 15 ) ;    
            //PB0,PB3,PB4,PB5
            GPIOB->CFGLR = ( GPIOB->CFGLR  & ( 
              ~( GPIO_CFGLR_MODE0 | GPIO_CFGLR_MODE3 | GPIO_CFGLR_MODE4 | GPIO_CFGLR_MODE5 | GPIO_CFGLR_CNF0 | GPIO_CFGLR_CNF3 | GPIO_CFGLR_CNF4 | GPIO_CFGLR_CNF5 ) ) )  
             | ( GPIO_CFGLR_CNF0_1 | GPIO_CFGLR_CNF3_1 | GPIO_CFGLR_CNF4_1 | GPIO_CFGLR_CNF5_1  ) ;
            //PB9,PB10,PB11,PB12,PB13,PB14,PB15
            GPIOB->CFGHR = ( GPIOB->CFGHR  & ( GPIO_CFGHR_MODE8 | GPIO_CFGHR_CNF9 ) )  
             | ( GPIO_CFGHR_CNF9_1 | GPIO_CFGHR_CNF10_1 | GPIO_CFGHR_CNF11_1 |GPIO_CFGHR_CNF12_1 | GPIO_CFGHR_CNF12_1 | GPIO_CFGHR_CNF13_1  
               | GPIO_CFGHR_CNF14_1 | GPIO_CFGHR_CNF15_1 ) ;
            GPIOB->BSHR = ( (uint16_t)0x0001 << 0 ) | ( (uint16_t)0x0001 << 3 ) | ( (uint16_t)0x0001 << 4 ) | ( (uint16_t)0x0001 << 5 ) 
             | ( (uint16_t)0x0001 << 9 ) | ( (uint16_t)0x0001 << 10 ) | ( (uint16_t)0x0001 << 11 ) | ( (uint16_t)0x0001 << 12 ) 
             | ( (uint16_t)0x0001 << 13 ) | ( (uint16_t)0x0001 << 14 ) | ( (uint16_t)0x0001 << 15 );
			      //PB2
            GPIOB->CFGLR = ( GPIOB->CFGLR  & ( ~( GPIO_CFGLR_MODE2 | GPIO_CFGLR_CNF2 ) ) ) | ( GPIO_CFGHR_CNF15_1 ) ;
            GPIOB->BCR = ( (uint16_t)0x0001 << 2 ) ;	
            //PC13,PC14,PC15
            GPIOC->CFGHR = ( GPIOB->CFGHR  & ( 
              ~( GPIO_CFGHR_MODE13 | GPIO_CFGHR_MODE14 | GPIO_CFGHR_MODE15 | GPIO_CFGHR_CNF13 | GPIO_CFGHR_CNF14 | GPIO_CFGHR_CNF15 ) ) )  
             | ( GPIO_CFGHR_CNF13_1 | GPIO_CFGHR_CNF14_1 | GPIO_CFGHR_CNF15_1  ) ;
            GPIOC->BSHR = ( (uint16_t)0x0001 << 13 ) | ( (uint16_t)0x0001 << 14 ) | ( (uint16_t)0x0001 << 15 );
            break;
        }
        case 0x10310700:     //CH32L103C8T6
        {
            break;
        }
        default:
        {
            break;
        }

    }
}
