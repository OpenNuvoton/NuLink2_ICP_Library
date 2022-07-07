/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Show how to use M030G ICP lib to erase/program/read target flash.
 *
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include "stdio.h"
#include "NuMicro.h"
#include "M030G_ICPLib.h"


#define PLL_CLOCK       192000000

#define SN_ADDRESS      (0x8000 - 4)


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
    //SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) | (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
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
    int32_t bEntry;
    uint32_t u32CID, u32DID, u32PID, u32UID[3], u32UCID[4];
    uint32_t u32Config_w[3], u32Config_r[3];
    uint32_t u32ImageSize, u32SN;
    uint32_t u32Result;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init Nu-Link2 LED */
    /* PF.4, PC.6, PB.8, PB.9 correspond to ICE, ISP, Green, Red respectively */
    GPIO_SetMode(PF, BIT4, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT6, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, BIT8 | BIT9, GPIO_MODE_OUTPUT);

    /* Control Nu-Link2-Pro Voltage Output */
    GPIO_SetMode(PB, BIT3 | BIT4, GPIO_MODE_OUTPUT);

    /* ICE LED is ON */
    PF4 = 0;
    PC6 = 1;
    PB8 = 1;
    PB9 = 1;

    PB3 = 0;
    PB4 = 1;

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);

    printf("+-------------------------------------+\n");
    printf("|    M030G ICP Library Sample Code    |\n");
    printf("+-------------------------------------+\n\n");

    while(PC7 != 0);

    /* ISP LED is ON */
    PF4 = 1;
    PC6 = 0;
    PB8 = 1;
    PB9 = 1;

    /* Init ICP */
    ICP_Init();

    /* Enter ICP mode */
    bEntry = ICP_ModeEntry();

    if (bEntry)
    {
        /* Read Nuvoton Company ID */
        u32CID = ICP_ReadCID();
        printf("CID: 0x%08X\n", u32CID);

        /* Read Device ID */
        u32DID = ICP_ReadDID();
        printf("DID: 0x%08X\n", u32DID);

        /* Read Part Device ID */
        u32PID = ICP_ReadPID();
        printf("PID: 0x%08X\n", u32PID);

        /* Read 96-bit Unique ID */
        u32UID[0] = ICP_ReadUID(0);
        u32UID[1] = ICP_ReadUID(1);
        u32UID[2] = ICP_ReadUID(2);
        printf("UID: 0x%08X-0x%08X-0x%08X\n", u32UID[0], u32UID[1], u32UID[2]);

        /* Read 128-bit Unique Customer ID */
        u32UCID[0] = ICP_ReadUCID(0);
        u32UCID[1] = ICP_ReadUCID(1);
        u32UCID[2] = ICP_ReadUCID(2);
        u32UCID[3] = ICP_ReadUCID(3);
        printf("UCID: 0x%08X-0x%08X-0x%08X-0x%08X\n", u32UCID[0], u32UCID[1], u32UCID[2], u32UCID[3]);

        /* Read CONFIG */
        ICP_ReadFlash(FMC_CONFIG_BASE + 0x00, 4, &u32Config_r[0]);
        ICP_ReadFlash(FMC_CONFIG_BASE + 0x04, 4, &u32Config_r[1]);
        ICP_ReadFlash(FMC_CONFIG_BASE + 0x08, 4, &u32Config_r[2]);
        printf("CONFIG0: 0x%08X, CONFIG1: 0x%08X, CONFIG2: 0x%08X\n", u32Config_r[0], u32Config_r[1], u32Config_r[2]);

        /* Mass Erase */
        ICP_MassErase();

        /* Exit ICP mode to Reload CONFIG */
        ICP_ModeExit();

        /* Re-enter ICP mode */
        ICP_ModeEntry();

        u32CID = ICP_ReadCID();
        printf("CID: 0x%08X\n", u32CID);

        /* Check if the target chip is in lock mode */
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
                u32Config_w[1] = 0x00007800;
                u32Config_w[2] = 0xFFFFFF5A;

                ICP_ProgramFlash(FMC_CONFIG_BASE + 0x00, 4, &u32Config_w[0]);
                ICP_ProgramFlash(FMC_CONFIG_BASE + 0x04, 4, &u32Config_w[1]);
                ICP_ProgramFlash(FMC_CONFIG_BASE + 0x08, 4, &u32Config_w[2]);

                /* Verify CONFIG */
                ICP_ReadFlash(FMC_CONFIG_BASE + 0x00, 4, &u32Config_r[0]);
                ICP_ReadFlash(FMC_CONFIG_BASE + 0x04, 4, &u32Config_r[1]);
                ICP_ReadFlash(FMC_CONFIG_BASE + 0x08, 4, &u32Config_r[2]);

                if ((u32Config_r[0] != u32Config_w[0]) || (u32Config_r[1] != u32Config_w[1]) || (u32Config_r[2] != u32Config_w[2]))
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
    }
    else
    {
        printf("ERROR: Failed to Enter ICP Mode!\n");
        i32Err = 3;
    }

    /* Exit ICP mode */
    ICP_ModeExit();

    /* Uninit ICP */
    ICP_UnInit();

    /* If passed, Green LED is ON; otherwise, Red LED is ON */
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
