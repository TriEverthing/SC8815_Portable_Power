/********************************** (C) COPYRIGHT *******************************
* File Name          : PD_Process.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/07/06
* Description        : This file contains all the functions prototypes for the
*                      PD library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef _USBPD_SINK_H_
#define _USBPD_SINK_H_

#ifdef __cplusplus
 extern "C" {
#endif

/******************************************************************************/
/* Related macro definitions */
/* Register Bit Definition */
/* USBPD->CONFIG */
#define PD_FILT_ED          (1<<0)            /* PD pin input filter enable */
#define PD_ALL_CLR          (1<<1)             /* Clear all interrupt flags */
#define CC_SEL              (1<<2)             /* Select PD communication port */
#define PD_DMA_EN           (1<<3)             /* Enable DMA for USBPD */
#define PD_RST_EN           (1<<4)             /* PD mode reset command enable */
#define WAKE_POLAR          (1<<5)             /* PD port wake-up level */
#define IE_PD_IO            (1<<10)            /* PD IO interrupt enable */
#define IE_RX_BIT           (1<<11)            /* Receive bit interrupt enable */
#define IE_RX_BYTE          (1<<12)            /* Receive byte interrupt enable */
#define IE_RX_ACT           (1<<13)            /* Receive completion interrupt enable */
#define IE_RX_RESET         (1<<14)            /* Reset interrupt enable */
#define IE_TX_END           (1<<15)            /* Transfer completion interrupt enable */

/* USBPD->CONTROL */
#define PD_TX_EN            (1<<0)             /* USBPD transceiver mode and transmit enable */
#define BMC_START           (1<<1)             /* BMC send start signal */
#define RX_STATE_0          (1<<2)             /* PD received state bit 0 */
#define RX_STATE_1          (1<<3)             /* PD received state bit 1 */
#define RX_STATE_2          (1<<4)             /* PD received state bit 2 */
#define DATA_FLAG           (1<<5)             /* Cache data valid flag bit */
#define TX_BIT_BACK         (1<<6)             /* Indicates the current bit status of the BMC when sending the code */
#define BMC_BYTE_HI         (1<<7)             /* Indicates the current half-byte status of the PD data being sent and received */

/* USBPD->TX_SEL */
#define TX_SEL1             (0<<0)
#define TX_SEL1_SYNC1       (0<<0)             /* 0-SYNC1 */
#define TX_SEL1_RST1        (1<<0)             /* 1-RST1 */
#define TX_SEL2_Mask        (3<<2)
#define TX_SEL2_SYNC1       (0<<2)             /* 00-SYNC1 */
#define TX_SEL2_SYNC3       (1<<2)             /* 01-SYNC3 */
#define TX_SEL2_RST1        (2<<2)             /* 1x-RST1 */
#define TX_SEL3_Mask        (3<<4)
#define TX_SEL3_SYNC1       (0<<4)             /* 00-SYNC1 */
#define TX_SEL3_SYNC3       (1<<4)             /* 01-SYNC3 */
#define TX_SEL3_RST1        (2<<4)             /* 1x-RST1 */
#define TX_SEL4_Mask        (3<<6)
#define TX_SEL4_SYNC2       (0<<6)             /* 00-SYNC2 */
#define TX_SEL4_SYNC3       (1<<6)             /* 01-SYNC3 */
#define TX_SEL4_RST2        (2<<6)             /* 1x-RST2 */

/* USBPD->STATUS */
#define BMC_AUX            (3<<0)              /* BMC auxiliary information */
#define BMC_AUX_INVALID    (0<<0)              /* 00-Invalid */
#define BMC_AUX_SOP0       (1<<0)              /* 01-SOP0 */
#define BMC_AUX_SOP1_HRST  (2<<0)              /* 10-SOP1 hard reset */
#define BMC_AUX_SOP2_CRST  (3<<0)              /* 11-SOP2 cable reset */
#define BUF_ERR            (1<<2)              /* BUFFER or DMA error interrupt flag */
#define IF_RX_BIT          (1<<3)              /* Receive bit or 5bit interrupt flag */
#define IF_RX_BYTE         (1<<4)              /* Receive byte or SOP interrupt flag */
#define IF_RX_ACT          (1<<5)              /* Receive completion interrupt flag */
#define IF_RX_RESET        (1<<6)              /* Receive reset interrupt flag */
#define IF_TX_END          (1<<7)              /* Transfer completion interrupt flag */

/* USBPD->PORT_CC1 */
/* USBPD->PORT_CC2 */
#define PA_CC_AI           (1<<0)               /* CC port comparator analogue input */
#define CC_PD              (1<<1)               /* CC port pull-down resistor enable */
#define CC_PU_Mask         (3<<2)               /* Clear CC port pull-up current  */
#define CC_NO_PU           (0<<2)               /* 00-Prohibit pull-up current */
#define CC_PU_330          (1<<2)               /* 01-330uA */
#define CC_PU_180          (2<<2)               /* 10-180uA */
#define CC_PU_80           (3<<2)               /* 11-80uA */
#define CC_LVE             (1<<4)               /* CC port output low voltage enable */
#define CC_CE              (7<<5)               /* Enable the voltage comparator on port CC */
#define CC_NO_CMP          (0<<5)               /* 000-closed */
#define CC_CMP_22          (2<<5)               /* 010-0.22V */
#define CC_CMP_45          (3<<5)               /* 011-0.45V */
#define CC_CMP_55          (4<<5)               /* 100-0.55V */
#define CC_CMP_66          (5<<5)               /* 101-0.66V */
#define CC_CMP_95          (6<<5)               /* 110-0.95V */
#define CC_CMP_123         (7<<5)               /* 111-1.23V */

#define USBPD_IN_HVT       (1<<9)
/*********************************************************
* PD pin PB6/PB7 high threshold input mode:
* 1: High threshold input (2.2V typical), to reduce the I/O power consumption during PD communication
* 0: Normal GPIO threshold input
* *******************************************************/

/* Control Message Types */
#define DEF_TYPE_RESERVED          0x00
#define DEF_TYPE_GOODCRC           0x01                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_GOTOMIN           0x02                                         /* Send By: Source */
#define DEF_TYPE_ACCEPT            0x03                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_REJECT            0x04                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_PING              0x05                                         /* Send By: Source */
#define DEF_TYPE_PS_RDY            0x06                                         /* Send By: Source,Sink */
#define DEF_TYPE_GET_SRC_CAP       0x07                                         /* Send By: Sink,DRP */
#define DEF_TYPE_GET_SNK_CAP       0x08                                         /* Send By: Source,DRP */
#define DEF_TYPE_DR_SWAP           0x09                                         /* Send By: Source,Sink */
#define DEF_TYPE_PR_SWAP           0x0A                                         /* Send By: Source,Sink */
#define DEF_TYPE_VCONN_SWAP        0x0B                                         /* Send By: Source,Sink */
#define DEF_TYPE_WAIT              0x0C                                         /* Send By: Source,Sink */
#define DEF_TYPE_SOFT_RESET        0x0D                                         /* Send By: Source,Sink */
#define DEF_TYPE_DATA_RESET        0x0E                                         /* Send By: Source,Sink */
#define DEF_TYPE_DATA_RESET_CMP    0x0F                                         /* Send By: Source,Sink */
#define DEF_TYPE_NOT_SUPPORT       0x10                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_GET_SRC_CAP_EX    0x11                                         /* Send By: Sink,DRP */
#define DEF_TYPE_GET_STATUS        0x12                                         /* Send By: Source,Sink */
#define DEF_TYPE_GET_STATUS_R      0X02                                         /* ext=1 */
#define DEF_TYPE_FR_SWAP           0x13                                         /* Send By: Sink */
#define DEF_TYPE_GET_PPS_STATUS    0x14                                         /* Send By: Sink */
#define DEF_TYPE_GET_CTY_CODES     0x15                                         /* Send By: Source,Sink */
#define DEF_TYPE_GET_SNK_CAP_EX    0x16                                         /* Send By: Source,DRP */
#define DEF_TYPE_GET_SRC_INFO      0x17                                         /* Send By: Sink,DRP */
#define DEF_TYPE_GET_REVISION      0x18                                         /* Send By: Source,Sink */

/* Data Message Types */
#define DEF_TYPE_SRC_CAP           0x01                                         /* Send By: Source,Dual-Role Power */
#define DEF_TYPE_REQUEST           0x02                                         /* Send By: Sink */
#define DEF_TYPE_BIST              0x03                                         /* Send By: Tester,Source,Sink */
#define DEF_TYPE_SNK_CAP           0x04                                         /* Send By: Sink,Dual-Role Power */
#define DEF_TYPE_BAT_STATUS        0x05                                         /* Send By: Source,Sink */
#define DEF_TYPE_ALERT             0x06                                         /* Send By: Source,Sink */
#define DEF_TYPE_GET_CTY_INFO      0x07                                         /* Send By: Source,Sink */
#define DEF_TYPE_ENTER_USB         0x08                                         /* Send By: DFP */
#define DEF_TYPE_EPR_REQUEST       0x09                                         /* Send By: Sink */
#define DEF_TYPE_EPR_MODE          0x0A                                         /* Send By: Source,Sink */
#define DEF_TYPE_SRC_INFO          0x0B                                         /* Send By: Source */
#define DEF_TYPE_REVISION          0x0C                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_VENDOR_DEFINED    0x0F                                         /* Send By: Source,Sink,Cable Plug */

/* Vendor Define Message Command */
#define DEF_VDM_DISC_IDENT         0x01
#define DEF_VDM_DISC_SVID          0x02
#define DEF_VDM_DISC_MODE          0x03
#define DEF_VDM_ENTER_MODE         0x04
#define DEF_VDM_EXIT_MODE          0x05
#define DEF_VDM_ATTENTION          0x06
#define DEF_VDM_DP_S_UPDATE        0x10
#define DEF_VDM_DP_CONFIG          0x11

/* PD Revision */
#define DEF_PD_REVISION_10         0x00
#define DEF_PD_REVISION_20         0x01
#define DEF_PD_REVISION_30         0x02


/* PD PHY Channel */
#define DEF_PD_CC1                 0x00
#define DEF_PD_CC2                 0x01

#define PIN_CC1                    ((uint16_t)0x0001 << 6 )
#define PIN_CC2                    ((uint16_t)0x0001 << 7 )

/* PD Tx Status */
#define DEF_PD_TX_OK               0x00
#define DEF_PD_TX_FAIL             0x01

/* PDO INDEX */
#define PDO_INDEX_1                1
#define PDO_INDEX_2                2
#define PDO_INDEX_3                3
#define PDO_INDEX_4                4
#define PDO_INDEX_5                5

/******************************************************************************/
#define UPD_TMR_TX_96M    (160-1)                                            /* timer value for USB PD BMC transmittal @Fsys=96MHz */
#define UPD_TMR_RX_96M    (240-1)                                            /* timer value for USB PD BMC receiving @Fsys=96MHz */
#define UPD_TMR_TX_48M    (80-1)                                             /* timer value for USB PD BMC transmittal @Fsys=48MHz */
#define UPD_TMR_RX_48M    (120-1)                                            /* timer value for USB PD BMC receiving @Fsys=48MHz */
#define UPD_TMR_TX_24M    (40-1)                                             /* timer value for USB PD BMC transmittal @Fsys=24MHz */
#define UPD_TMR_RX_24M    (60-1)                                             /* timer value for USB PD BMC receiving @Fsys=24MHz */
#define UPD_TMR_TX_12M    (20-1)                                             /* timer value for USB PD BMC transmittal @Fsys=12MHz */
#define UPD_TMR_RX_12M    (30-1)                                             /* timer value for USB PD BMC receiving @Fsys=12MHz */

#define MASK_PD_STAT      0x03                                               /* Bit mask for current PD status */
#define PD_RX_SOP0        0x01                                               /* SOP0 received */
#define PD_RX_SOP1_HRST   0x02                                               /* SOP1 or Hard Reset received */
#define PD_RX_SOP2_CRST   0x03                                               /* SOP2 or Cable Reset received */

#define UPD_SOP0          ( TX_SEL1_SYNC1 | TX_SEL2_SYNC1 | TX_SEL3_SYNC1 | TX_SEL4_SYNC2 )     /* SOP1 */
#define UPD_SOP1          ( TX_SEL1_SYNC1 | TX_SEL2_SYNC1 | TX_SEL3_SYNC3 | TX_SEL4_SYNC3 )     /* SOP2 */
#define UPD_SOP2          ( TX_SEL1_SYNC1 | TX_SEL2_SYNC3 | TX_SEL3_SYNC1 | TX_SEL4_SYNC3 )     /* SOP3 */
#define UPD_HARD_RESET    ( TX_SEL1_RST1  | TX_SEL2_RST1  | TX_SEL3_RST1  | TX_SEL4_RST2  )     /* Hard Reset*/
#define UPD_CABLE_RESET   ( TX_SEL1_RST1  | TX_SEL2_SYNC1 | TX_SEL3_RST1  | TX_SEL4_SYNC3 )     /* Cable Reset*/


#define bCC_CMP_22        0x01
#define bCC_CMP_45        0x02
#define bCC_CMP_55        0x04
#define bCC_CMP_66        0x08
#define bCC_CMP_95        0x10
#define bCC_CMP_123       0x20
#define bCC_CMP_220       0x40

/******************************************************************************/
/* PD State Machine */
typedef enum
{
    STA_IDLE = 0,                                                               /* 0: No task status */
    STA_DISCONNECT,                                                             /* 1: Disconnection */
    STA_SRC_CONNECT,                                                            /* 2: SRC connect */
    STA_RX_SRC_CAP_WAIT,                                                        /* 3: Waiting to receive SRC_CAP */
    STA_RX_SRC_CAP,                                                             /* 4: SRC_CAP received */
    STA_TX_REQ,                                                                 /* 5: Send REQUEST */
    STA_RX_ACCEPT_WAIT,                                                         /* 6: Waiting to receive ACCEPT */
    STA_RX_ACCEPT,                                                              /* 7: ACCEPT received */
    STA_RX_REJECT,                                                              /* 8: REJECT received */
    STA_RX_PS_RDY_WAIT,                                                         /* 9: Waiting to receive PS_RDY */
    STA_RX_PS_RDY,                                                              /* 10: PS_RDY received */
    STA_SINK_CONNECT,                                                           /* 11: SNK access */
    STA_TX_SRC_CAP,                                                             /* 12: Send SRC_CAP */
    STA_RX_REQ_WAIT,                                                            /* 13: Waiting to receive REQUEST */
    STA_RX_REQ,                                                                 /* 14: REQUEST received */
    STA_TX_ACCEPT,                                                              /* 15: Send ACCEPT */
    STA_TX_REJECT,                                                              /* 16: Send REJECT */
    STA_ADJ_VOL,                                                                /* 17: Adjustment of output voltage and current */
    STA_TX_PS_RDY,                                                              /* 18: Send PS_RDY */
    STA_TX_DR_SWAP,                                                             /* 19: Send DR_SWAP */
    STA_RX_DR_SWAP_ACCEPT,                                                      /* 20: Waiting to receive the answer ACCEPT from DR_SWAP */
    STA_TX_PR_SWAP,                                                             /* 21: Send PR_SWAP */
    STA_RX_PR_SWAP_ACCEPT,                                                      /* 22: Waiting to receive the answer ACCEPT from PR_SWAP */
    STA_RX_PR_SWAP_PS_RDY,                                                      /* 23: Waiting to receive the answer PS_RDY from PR_SWAP */
    STA_TX_PR_SWAP_PS_RDY,                                                      /* 24: Send answer PS_RDY for PR_SWAP */
    STA_PR_SWAP_RECON_WAIT,                                                     /* 25: Wait for PR_SWAP before reconnecting */
    STA_SRC_RECON_WAIT,                                                         /* 26: Waiting for SRC to reconnect */
    STA_SINK_RECON_WAIT,                                                        /* 27: Waiting for SNK to reconnect */
    STA_RX_APD_PS_RDY_WAIT,                                                     /* 28: Waiting for PS_RDY from the receiving adapter */
    STA_RX_APD_PS_RDY,                                                          /* 29: PS_RDY received from the adapter */
    STA_MODE_SWITCH,                                                            /* 30: Mode switching */
    STA_TX_SOFTRST,                                                             /* 31: Sending a software reset */
    STA_TX_HRST,                                                                /* 32: Send hardware reset */
    STA_PHY_RST,                                                                /* 33: PHY reset */
    STA_APD_IDLE_WAIT,                                                          /* 34: Waiting for the adapter to become idle */
} CC_STATUS;

/******************************************************************************/
/* PD Message Header Struct */
typedef union
{
    struct _Message_Header
    {
        uint8_t  MsgType: 5;                                                      /* Message Type */
        uint8_t  PDRole: 1;                                                       /* 0-UFP; 1-DFP */
        uint8_t  SpecRev: 2;                                                      /* 00-Rev1.0; 01-Rev2.0; 10-Rev3.0; */
        uint8_t  PRRole: 1;                                                       /* 0-Sink; 1-Source */
        uint8_t  MsgID: 3;
        uint8_t  NumDO: 3;
        uint8_t  Ext: 1;
    }Message_Header;
    uint16_t Data;
}_Message_Header;

/******************************************************************************/
/* Bit definition */
typedef union
{
    struct _BITS_
    {
        uint8_t  Msg_Recvd: 1;                                                    /* Notify the main program of the receipt of a PD packet */
        uint8_t  Connected: 1;                                                    /* PD Physical Layer Connected Flag */
        uint8_t  Stop_Det_Chk: 1;                                                 /* 0-Enable detection; 1-Disable disconnection detection */
        uint8_t  PD_Role: 1;                                                      /* 0-UFP; 1-DFP */
        uint8_t  PR_Role: 1;                                                      /* 0-Sink; 1-Source */
        uint8_t  Auto_Ack_PRRole: 1;                                              /* Role used by auto-responder 0:SINK; 1:SOURCE */
        uint8_t  PD_Version: 1;                                                   /* PD version 0-PD2.0; 1-PD3.0 */
        uint8_t  VDM_Version: 1;                                                  /* VDM Version 0-1.0 1-2.0 */
        uint8_t  HPD_Connected: 1;                                                /* HPD Physical Layer Connected Flag */
        uint8_t  HPD_Det_Chk: 1;                                                  /* 0-turn off HPD connection detection; 1-turn on HPD connection detection */
        uint8_t  CC_Sel_En: 1;                                                    /* 0-CC channel selection toggle enable; 1-CC channel selection toggle disable */
        uint8_t  CC_Sel_State: 1;                                                 /* 0-CC channel selection switches to 0; 1-CC channel selection switches to 1 */
        uint8_t  PD_Comm_Succ: 1;                                                 /* 0-PD communication unsuccessful; 1-PD communication successful; */
        uint8_t  Recv: 3;
    }Bit;
    uint16_t Bit_Flag;
}_BIT_FLAG;

typedef struct 
{
  uint16_t Current;
  uint16_t Voltage;
} FixedSourceCap_t;

typedef struct  
{
  uint16_t MinVoltage;
  uint16_t MaxVoltage;
  uint16_t Current;
} PPSSourceCap_t;

/* PD control-related structures */
typedef struct _PD_CONTROL
{
    CC_STATUS PD_State;                                                         /* PD communication status machine */
    CC_STATUS PD_State_Last;                                                    /* PD communication status machine (last value) */
    uint8_t  Msg_ID;                                                              /* ID of the message sent */
    uint8_t  Det_Timer;                                                           /* PD connection status detection timing */
    uint8_t  Det_Cnt;                                                             /* Number of PD connection status detections */
    uint8_t  Det_Sel_Cnt;                                                         /* Number of SEL toggles for PD connection status detection */
    uint8_t  HPD_Det_Timer;                                                       /* HPD connection detection timing */
    uint8_t  HPD_Det_Cnt;                                                         /* HPD pin connection status detection count */
    uint16_t PD_Comm_Timer;                                                       /* PD shared timing variables */
    uint8_t  ReqPDO_Idx;                                                          /* Index of the requested PDO, valid values 1-7 */
    uint16_t PD_BusIdle_Timer;                                                    /* Bus Idle Time Timer */
    uint8_t  Mode_Try_Cnt;                                                        /* Number of retries for current mode, highest bit marks mode */
    uint8_t  Err_Op_Cnt;                                                          /* Exception operation count */
    uint8_t  Adapter_Idle_Cnt;                                                    /* Adapter communication idle timing */
    _BIT_FLAG Flag;                                                             /* Flag byte bit definition */
}PD_CONTROL, *pPD_CONTROL;

/* Variable extents */
extern uint8_t  Tmr_Ms_Cnt_Last;
extern uint8_t  Tmr_Ms_Dlt;

extern uint8_t  PDO_Len;
extern PD_CONTROL PD_Ctl;

extern uint8_t send_data[ ];
extern uint8_t PD_Ack_Buf[ ];

extern __attribute__ ((aligned(4))) uint8_t USBPD_Rx_Buf[ 34 ];
extern __attribute__ ((aligned(4))) uint8_t USBPD_Tx_Buf[ 34 ];


/***********************************************************************************************************************/
/* Function extensibility */
extern void USBPD_Rx_Mode( void );
extern void USBPD_Src_Init( void );
extern void USBPD_Sink_Init( void );
extern void USBPD_PHY_Reset( void );
extern void usbpd_init( void );
extern uint8_t USBPD_Detect( void );
extern void USBPD_Det_Proc( void );
extern void USBPD_Load_Header( uint8_t ex, uint8_t msg_type );
extern uint8_t USBPD_Send_Handle( uint8_t *pbuf, uint8_t len );
extern void USBPD_Phy_SendPack( uint8_t mode, uint8_t *pbuf, uint8_t len, uint8_t sop );
extern void USBPD_Main_Proc( void );
extern void USBPD_PDO_Analyse( uint8_t pdo_idx, uint8_t *srccap, uint16_t *current, uint16_t *voltage );

#if USE_FreeRTOS
void USBPD_ISR_Task(void *pvParameters);
void USBPD_Task(void *pvParameters);
#else
void usbpd_loop( void );
#endif

#ifdef __cplusplus
}
#endif

#endif /* USER_PD_PROCESS_H_ */
