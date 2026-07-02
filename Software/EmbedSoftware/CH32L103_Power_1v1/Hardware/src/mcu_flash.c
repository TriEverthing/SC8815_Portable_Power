#include "ch32l103x_bsp.h"
#include "mcu_flash.h"

//CH32L103 MCU Size infromation , 0x08000000-0x0800FFFF
#define FLASH_ROM_SIZE          32      //64K
#define FLASH_PAGE_SIZE         256     //256Byte
//User Flash Addr
#define FLASH_ROM_WRITE_ADDR    (FLASH_BASE + FLASH_ROM_SIZE * 1024 - FLASH_PAGE_SIZE)
#define FMC_WRITE_END_ADDR      (FLASH_ROM_WRITE_ADDR + FLASH_PAGE_SIZE)

/* FLASH Keys */
#define CH32_RDP_Key            ((uint16_t)0x00A5)
#define CH32_FLASH_KEY1         ((uint32_t)0x45670123)
#define CH32_FLASH_KEY2         ((uint32_t)0xCDEF89AB)

uint8_t flash_data[FLASH_PAGE_SIZE] = {0};


void Flash_Unlock_Fast( void )
{
    /* Authorize the FPEC of Bank1 Access */
    FLASH->KEYR = CH32_FLASH_KEY1;
    FLASH->KEYR = CH32_FLASH_KEY2;

    /* Fast program mode unlock */
    FLASH->MODEKEYR = CH32_FLASH_KEY1;
    FLASH->MODEKEYR = CH32_FLASH_KEY2;    
}

void Flash_Lock_Fast( void )
{
    FLASH->CTLR |= FLASH_CTLR_FLOCK;
}

void Flash_ErasePage_Fast(uint32_t Page_Address)
{
    FLASH->CTLR &= ( ~( FLASH_CTLR_OBER | FLASH_CTLR_FTER ) ) ;
	
	FLASH->CTLR |= FLASH_CTLR_FTER ;
    FLASH->ADDR = Page_Address;
    FLASH->CTLR |= FLASH_CTLR_STRT ;
    while(FLASH->STATR & FLASH_STATR_BSY );
    FLASH->CTLR &= ~FLASH_CTLR_FTER ; 
}

void Flash_ProgramPage_Fast(uint32_t Address , uint32_t *Data0  , uint16_t length )
{
    uint16_t i ;
    //FLASH_BufLoad
    FLASH->CTLR &= ( ~( FLASH_CTLR_OBER | FLASH_CTLR_FTER ) ) ;
	
	FLASH->CTLR |= FLASH_CTLR_FTPG ;

    for( i = 0 ; i < length ; i = i + 4 )
    {
        *(__IO uint32_t *)(Address) = *Data0 ;
        FLASH->CTLR |= FLASH_CTLR_BUFLOAD ;
        while( FLASH->STATR & FLASH_STATR_BSY );
        Data0 = Data0 + 4 ;
        Address = Address + 4 ;
    }

    FLASH->CTLR &= ~FLASH_CTLR_FTPG ;

    FLASH->CTLR &= ( ~( FLASH_CTLR_OBER | FLASH_CTLR_FTER ) ) ;
	FLASH->CTLR |= FLASH_CTLR_FTPG ;
    FLASH->ADDR = Address & 0xFFFFF00 ;
    FLASH->CTLR |= FLASH_CTLR_STRT ;
    while(FLASH->STATR & FLASH_STATR_BSY );
    FLASH->CTLR &= ~FLASH_CTLR_FTPG ;
}

void flash_program_page(uint32_t address , uint32_t* data)
{
    Flash_Unlock_Fast();

    /* pages program flash */
    Flash_ProgramPage_Fast( address , data , FLASH_PAGE_SIZE );

    /* lock the main FMC after the program operation */
    Flash_Lock_Fast();
}

void mcu_flash_erase( void )
{
    Flash_Unlock_Fast();

    Flash_ErasePage_Fast(FLASH_ROM_WRITE_ADDR);

    Flash_Lock_Fast();
}

void mcu_flash_read(uint8_t *data , uint16_t len)
{
    uint32_t read_data = 0;
    uint32_t* addr = (uint32_t *)FLASH_ROM_WRITE_ADDR;
    int j = 0;
    for (int i = 0; i < len; i++)
    {
        read_data = *addr;
        data[j] = read_data & 0xff;
        data[j+1] = (read_data >> 8) & 0xff;
        data[j+2] = (read_data >> 16) & 0xff;
        data[j+3] = (read_data >> 24) & 0xff;
        addr++;
        j += 4;
    }
}

void mcu_flash_write(uint8_t *data , uint16_t len)
{
    uint32_t addr = FLASH_ROM_WRITE_ADDR;
    for (int i = 0; i < FLASH_PAGE_SIZE; i++)
    {
        flash_data[i] = *(uint8_t *)addr;
        addr++;
    }
    for (int i = 0; i < len; i++)
    {
        flash_data[i] = data[i];
    }
    mcu_flash_erase();
    flash_program_page(FLASH_ROM_WRITE_ADDR, (uint32_t *)flash_data);
}