/**************************************************************************//**
 * @file     ICPLib.h
 * @version  V1.0
 * @brief    M480 Series ICP Lib Header File
 *
 * @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#define ICP_RESET_ACTIVE_LOW    0x0UL
#define ICP_RESET_ACTIVE_HIGH   0x1UL


/* ICP Command */
uint32_t ICP_GetVersion(void);

void ICP_Init(uint32_t u32ResetActiveLevel);
void ICP_UnInit(void);

void ICP_SetMassEraseTime(uint32_t u32DelayTime, uint32_t u32HoldTime);
void ICP_SetPageEraseTime(uint32_t u32DelayTime, uint32_t u32HoldTime);
void ICP_SetProgramTime(uint32_t u32DelayTime, uint32_t u32HoldTime);

void ICP_ModeEntry(void);
void ICP_ModeExit(void);

uint32_t ICP_ReadCID(void);
uint32_t ICP_ReadDID(void);
uint32_t ICP_ReadPID(void);
uint32_t ICP_ReadUID(uint32_t u32Index);
uint32_t ICP_ReadUCID(uint32_t u32Index);

void ICP_MassErase(void);
void ICP_PageEraseFlash(uint32_t u32Addr);
void ICP_SPROMErase(uint32_t u32Addr);
void ICP_XOMErase(uint32_t u32Addr, uint32_t u32PageCount);

uint32_t ICP_ProgramFlash(uint32_t u32StartAddr, uint32_t u32ByteSize, uint32_t *u32Buffer);
uint32_t ICP_ProgramVerifyFlash(uint32_t u32StartAddr, uint32_t u32ByteSize, uint32_t *u32Buffer);

void ICP_ProgramConfigWithCRC(uint32_t u32Config0, uint32_t u32Config1);    // For NUC442/NUC472 series

uint32_t ICP_ReadFlash(uint32_t u32StartAddr, uint32_t u32ByteSize, uint32_t *u32Buffer);

/*** (C) COPYRIGHT 2019 Nuvoton Technology Corp. ***/
