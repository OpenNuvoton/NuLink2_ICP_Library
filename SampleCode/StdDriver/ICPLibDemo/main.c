/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Show how to use ICP lib to erase/program/read target flash.
 *
 * @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include "stdio.h"
#include "NuMicro.h"
#include "ICPLib.h"


#define PLL_CLOCK       192000000

#define SN_ADDRESS      (0x4000 - 4)


extern uint32_t LDROMImageBase;
extern uint32_t LDROMImageLimit;
extern uint32_t APROMImageBase;
extern uint32_t APROMImageLimit;


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
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) | (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
}

void UART0_Init()
{
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

int32_t main(void)
{
    int32_t i32Err = 0;
    uint32_t u32CID, u32DID, u32PID, u32UID[3], u32UCID[4];
    uint32_t u32Config_w[2], u32Config_r[2];
    uint32_t u32ImageSize, u32SN;
    uint32_t u32Result;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    GPIO_SetMode(PF, BIT4, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT6, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, BIT8 | BIT9, GPIO_MODE_OUTPUT);

    PF4 = 0;
    PC6 = 1;
    PB8 = 1;
    PB9 = 1;

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);

    printf("+-------------------------------+\n");
    printf("|    ICP Library Sample Code    |\n");
    printf("+-------------------------------+\n\n");

    while(PC7 != 0);

    PF4 = 1;
    PC6 = 0;
    PB8 = 1;
    PB9 = 1;

    ICP_Init(ICP_RESET_ACTIVE_LOW);

    ICP_ModeEntry();

    //ICP_SetMassEraseTime(320000, 120);
    //ICP_SetPageEraseTime(120000, 120);
    //ICP_SetProgramTime(28, 10);

    u32CID = ICP_ReadCID();
    printf("CID: 0x%08X\n", u32CID);

    u32PID = ICP_ReadPID();
    printf("PID: 0x%08X\n", u32PID);

    u32DID = ICP_ReadDID();
    printf("DID: 0x%08X\n", u32DID);

    u32UID[0] = ICP_ReadUID(0);
    u32UID[1] = ICP_ReadUID(1);
    u32UID[2] = ICP_ReadUID(2);
    printf("UID: 0x%08X-0x%08X-0x%08X\n", u32UID[0], u32UID[1], u32UID[2]);

    u32UCID[0] = ICP_ReadUCID(0);
    u32UCID[1] = ICP_ReadUCID(1);
    u32UCID[2] = ICP_ReadUCID(2);
    u32UCID[3] = ICP_ReadUCID(3);
    printf("UCID: 0x%08X-0x%08X-0x%08X-0x%08X\n", u32UCID[0], u32UCID[1], u32UCID[2], u32UCID[3]);

    ICP_ReadFlash(FMC_CONFIG_BASE + 0x00, 4, &u32Config_r[0]);
    ICP_ReadFlash(FMC_CONFIG_BASE + 0x04, 4, &u32Config_r[1]);
    printf("CONFIG0: 0x%08X, CONFIG1: 0x%08X\n", u32Config_r[0], u32Config_r[1]);

    /* Mass Erase */
    ICP_MassErase();

    ICP_ModeExit();     // Reload CONFIG

    ICP_ModeEntry();

    u32CID = ICP_ReadCID();
    printf("CID: 0x%08X\n", u32CID);

    if (u32CID == 0xDA)
    {
        if (i32Err == 0)
        {
            /* Program and Verify APROM */
            u32ImageSize = (uint32_t)&APROMImageLimit - (uint32_t)&APROMImageBase;

            if (u32ImageSize)
            {
                u32Result = ICP_ProgramVerifyFlash(FMC_APROM_BASE, u32ImageSize, (uint32_t *)&APROMImageBase);

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
                u32Result = ICP_ProgramVerifyFlash(FMC_LDROM_BASE, u32ImageSize, (uint32_t *)&LDROMImageBase);

                if (u32Result != (FMC_LDROM_BASE + u32ImageSize))
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
            /* Program SN */
            u32SN = 0x12345678;
            ICP_ProgramFlash(SN_ADDRESS, 4, &u32SN);

            /* Update CONFIG */
            u32Config_w[0] = 0xFFFFFFFE;
            u32Config_w[1] = 0x00019000;

#if 1
            ICP_ProgramFlash(FMC_CONFIG_BASE + 0x00, 4, &u32Config_w[0]);
            ICP_ProgramFlash(FMC_CONFIG_BASE + 0x04, 4, &u32Config_w[1]);
#else
            // NUC442/NUC472
            ICP_ProgramConfigWithCRC(u32Config_w[0], u32Config_w[1]);
#endif

            ICP_ReadFlash(FMC_CONFIG_BASE + 0x00, 4, &u32Config_r[0]);
            ICP_ReadFlash(FMC_CONFIG_BASE + 0x04, 4, &u32Config_r[1]);

            if ((u32Config_r[0] != u32Config_w[0]) || (u32Config_r[1] != u32Config_w[1]))
            {
                printf("  ERROR: Failed to Update CONFIG!\n");
                i32Err = 1;
            }
        }
    }
    else
    {
        printf("Flash may be in Security Lock Mode\n");
        i32Err = 2;
    }

    ICP_ModeExit();

    ICP_UnInit();

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
