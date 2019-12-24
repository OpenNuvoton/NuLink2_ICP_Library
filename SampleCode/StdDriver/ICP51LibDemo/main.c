/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Show how to use ICP51 lib to erase/program/read target flash.
 *
 * @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include "stdio.h"
#include "NuMicro.h"
#include "ICP51Lib.h"


#define PLL_CLOCK           192000000

#define SN_ADDRESS          (0x1000 - 4)

#define TARGET_FLASH_SIZE   (32 * 1024)

#define TARGET_LDROM_SIZE   (4 * 1024)
#define TARGET_SPROM_SIZE   (128)

#define TARGET_LDROM_ADDR   (TARGET_FLASH_SIZE - TARGET_LDROM_SIZE)


extern uint32_t LDROMImageBase;
extern uint32_t LDROMImageLimit;
extern uint32_t APROMImageBase;
extern uint32_t APROMImageLimit;
extern uint32_t SPROMImageBase;
extern uint32_t SPROMImageLimit;


void SYS_Init(void)
{
    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    /* Enable HXT clock (external XTAL 12MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Wait for HXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Set PCLK0/PCLK1 to HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA0MFP_Msk | SYS_GPA_MFPL_PA1MFP_Msk)) | (SYS_GPA_MFPL_PA0MFP_UART0_RXD | SYS_GPA_MFPL_PA1MFP_UART0_TXD);
}

void UART0_Init()
{
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

int32_t main(void)
{
    int32_t i32Err = 0;
    uint8_t u8CID, u8UID[12], u8UCID[16];
    uint16_t u16DID, u16PID;
    uint8_t u8Config_w[5], u8Config_r[5];
    uint32_t u32ImageSize, u32SN;
    uint32_t i, u32Result;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    GPIO_SetMode(PF, BIT4, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT6, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, BIT8 | BIT9, GPIO_MODE_OUTPUT);

    GPIO_SetMode(PB, BIT3 | BIT4, GPIO_MODE_OUTPUT);

    PF4 = 0;
    PC6 = 1;
    PB8 = 1;
    PB9 = 1;

    PB3 = 0;
    PB4 = 1;

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);

    printf("+---------------------------------+\n");
    printf("|    ICP51 Library Sample Code    |\n");
    printf("+---------------------------------+\n\n");

    while(PC7 != 0);

    PF4 = 1;
    PC6 = 0;
    PB8 = 1;
    PB9 = 1;

    ICP51_Init();

    ICP51_ModeEntry();

    u8CID = ICP51_ReadCID();
    printf("CID: 0x%02X\n", u8CID);

    u16PID = ICP51_ReadPID();
    printf("PID: 0x%04X\n", u16PID);

    u16DID = ICP51_ReadDID();
    printf("DID: 0x%04X\n", u16DID);

    for (i = 0; i < 12; i++)
    {
        u8UID[i] = ICP51_ReadUID(i);
    }

    printf("UID 0-3: 0x%02X-0x%02X-0x%02X-0x%02X\n", u8UID[0], u8UID[1], u8UID[2], u8UID[3]);
    printf("UID 4-7: 0x%02X-0x%02X-0x%02X-0x%02X\n", u8UID[4], u8UID[5], u8UID[6], u8UID[7]);
    printf("UID 8-11: 0x%02X-0x%02X-0x%02X-0x%02X\n", u8UID[8], u8UID[9], u8UID[10], u8UID[11]);

    for (i = 0; i < 16; i++)
    {
        u8UCID[i] = ICP51_ReadUCID(i);
    }

    printf("UCID 0-3: 0x%02X-0x%02X-0x%02X-0x%02X\n", u8UCID[0], u8UCID[1], u8UCID[2], u8UCID[3]);
    printf("UCID 4-7: 0x%02X-0x%02X-0x%02X-0x%02X\n", u8UCID[4], u8UCID[5], u8UCID[6], u8UCID[7]);
    printf("UCID 8-11: 0x%02X-0x%02X-0x%02X-0x%02X\n", u8UCID[8], u8UCID[9], u8UCID[10], u8UCID[11]);
    printf("UCID 12-15: 0x%02X-0x%02X-0x%02X-0x%02X\n", u8UCID[12], u8UCID[13], u8UCID[14], u8UCID[15]);

    ICP51_ReadCFG(u8Config_r);
    printf("CONFIG: 0x%02X-0x%02X-0x%02X-0x%02X-0x%02X\n", u8Config_r[0], u8Config_r[1], u8Config_r[2], u8Config_r[3], u8Config_r[4]);

    /* Mass Erase */
    ICP51_MassErase();

    if (u8CID == 0xFF)
    {
        ICP51_ModeReEntry();    // Flash Reload

        u8CID = ICP51_ReadCID();
        printf("CID: 0x%02X\n", u8CID);
    }

    if (u8CID == 0xDA)
    {
        if (i32Err == 0)
        {
            /* Program and Verify APROM */
            u32ImageSize = (uint32_t)&APROMImageLimit - (uint32_t)&APROMImageBase;

            if (u32ImageSize)
            {
                u32Result = ICP51_ProgramFlash(FMC_APROM_BASE, u32ImageSize, (uint8_t *)&APROMImageBase, TRUE);

                if (u32Result != (FMC_APROM_BASE + u32ImageSize))
                {
                    printf("  ERROR: Failed to Update APROM!\n");
                    i32Err = 1;
                }
            }
            else
            {
                printf("  ERROR: No APROM Image!\n");
                i32Err = 1;
            }
        }

        if (i32Err == 0)
        {
            /* Program and Verify LDROM */
            u32ImageSize = (uint32_t)&LDROMImageLimit - (uint32_t)&LDROMImageBase;

            if (u32ImageSize)
            {
                u32Result = ICP51_ProgramFlash(TARGET_LDROM_ADDR, u32ImageSize, (uint8_t *)&LDROMImageBase, TRUE);

                if (u32Result != (TARGET_LDROM_ADDR + u32ImageSize))
                {
                    printf("  ERROR: Failed to Update LDROM!\n");
                    i32Err = 1;
                }
            }
            else
            {
                printf("  ERROR: No LDROM Image!\n");
                i32Err = 1;
            }
        }

        if (i32Err == 0)
        {
            /* Program and Verify SPROM */
            u32ImageSize = (uint32_t)&SPROMImageLimit - (uint32_t)&SPROMImageBase;

            if (u32ImageSize)
            {
                uint8_t u8Data;

                ICP51_ReadSPROM(TARGET_SPROM_SIZE - 1, 1, &u8Data);
                printf("SPROM Last Byte: 0x%02X\n", u8Data);

                /* Erase SPROM */
                ICP51_EraseSPROM();

                if (u8Data != 0xFF)
                {
                    ICP51_ModeReEntry();    // Flash Reload
                }

                u32Result = ICP51_ProgramSPROM(0, u32ImageSize, (uint8_t *)&SPROMImageBase, TRUE);

                if (u32Result != (0 + u32ImageSize))
                {
                    printf("  ERROR: Failed to Update SPROM!\n");
                    i32Err = 1;
                }
            }
            else
            {
                printf("  ERROR: No SPROM Image!\n");
                i32Err = 1;
            }
        }

        if (i32Err == 0)
        {
            /* Program SN */
            u32SN = 0x12345678;
            ICP51_ProgramFlash(SN_ADDRESS, 4, (uint8_t *)&u32SN, FALSE);

            /* Update CONFIG */
            u8Config_w[0] = 0xEF;
            u8Config_w[1] = 0xFB;
            u8Config_w[2] = 0xFB;
            u8Config_w[3] = 0xFF;
            u8Config_w[4] = 0xFF;

            ICP51_ProgramCFG(u8Config_w);

            ICP51_ReadCFG(u8Config_r);

            for (i = 0; i < 5; i++)
            {
                if (u8Config_r[i] != u8Config_w[i])
                {
                    printf("  ERROR: Failed to Update CONFIG!\n");
                    i32Err = 1;
                    break;
                }
            }
        }
    }
    else
    {
        printf("Flash may be in Security Lock Mode\n");
        i32Err = 2;
    }

    ICP51_ModeExit();

    ICP51_UnInit();

    if (i32Err == 0)
    {
        PF4 = 1;
        PC6 = 1;
        PB8 = 0;
        PB9 = 1;
    }
    else
    {
        PF4 = 1;
        PC6 = 1;
        PB8 = 1;
        PB9 = 0;
    }

    while(1);
}
