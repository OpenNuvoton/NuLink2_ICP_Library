/**************************************************************************//**
 * @file     M0A21_ICPLib.h
 * @version  V1.0
 * @brief    M480 Series M0A21 ICP Lib Header File
 *
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#define ICP_RESET_PIN_RESET_MODE    0x0UL
#define ICP_RESET_PIN_GPIO_MODE     0x1UL


/* ICP Command */
uint32_t ICP_GetVersion(void);

void ICP_Init(void);
void ICP_UnInit(void);

int32_t ICP_ModeEntry(uint32_t u32Mode);
void ICP_ModeExit(void);

uint32_t ICP_ReadCID(void);
uint32_t ICP_ReadDID(void);
uint32_t ICP_ReadPID(void);
uint32_t ICP_ReadUID(uint32_t u32Index);
uint32_t ICP_ReadUCID(uint32_t u32Index);

void ICP_MassErase(void);
void ICP_PageEraseFlash(uint32_t u32Addr);

uint32_t ICP_ProgramFlash(uint32_t u32StartAddr, uint32_t u32ByteSize, uint32_t *u32Buffer);
uint32_t ICP_ProgramVerifyFlash(uint32_t u32StartAddr, uint32_t u32ByteSize, uint32_t *u32Buffer);

uint32_t ICP_ReadFlash(uint32_t u32StartAddr, uint32_t u32ByteSize, uint32_t *u32Buffer);

/*** (C) COPYRIGHT 2021 Nuvoton Technology Corp. ***/
