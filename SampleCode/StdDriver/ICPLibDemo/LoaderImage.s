;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/


    AREA _image, DATA, READONLY

    EXPORT  LDROMImageBase
    EXPORT  LDROMImageLimit
    EXPORT  APROMImageBase
    EXPORT  APROMImageLimit 
 
    ALIGN   4     
LDROMImageBase
    INCBIN .\BinaryFile.bin
LDROMImageLimit

    ALIGN   4
APROMImageBase
    INCBIN .\Smpl_HID.bin
APROMImageLimit
    
    END