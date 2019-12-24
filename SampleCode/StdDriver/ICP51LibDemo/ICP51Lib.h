/**************************************************************************//**
 * @file     ICP51Lib.h
 * @version  V1.0
 * @brief    M480 Series ICP51 Lib Header File
 *
 * @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/


/* ICP Command */
uint32_t ICP51_GetVersion(void);

void ICP51_Init(void);
void ICP51_UnInit(void);

void ICP51_ModeEntry(void);
void ICP51_ModeExit(void);
void ICP51_ModeReEntry(void);

uint8_t ICP51_ReadCID(void);
uint16_t ICP51_ReadDID(void);
uint16_t ICP51_ReadPID(void);
uint8_t ICP51_ReadUID(uint32_t u32Index);
uint8_t ICP51_ReadUCID(uint32_t u32Index);

void ICP51_MassErase(void);
void ICP51_PageEraseFlash(uint32_t u32Addr);
uint32_t ICP51_ProgramFlash(uint32_t u32StartAddr, uint32_t u32ByteSize, uint8_t *u8Buffer, uint32_t bVerify);
uint32_t ICP51_ReadFlash(uint32_t u32StartAddr, uint32_t u32ByteSize, uint8_t *u8Buffer);

void ICP51_EraseCFG(void);
void ICP51_ProgramCFG(uint8_t *u8Buffer);
void ICP51_ReadCFG(uint8_t *u8Buffer);

void ICP51_EraseSPROM(void);
uint32_t ICP51_ProgramSPROM(uint32_t u32Offset, uint32_t u32ByteSize, uint8_t *u8Buffer, uint32_t bVerify);
uint32_t ICP51_ReadSPROM(uint32_t u32Offset, uint32_t u32ByteSize, uint8_t *u8Buffer);


/*** (C) COPYRIGHT 2019 Nuvoton Technology Corp. ***/
