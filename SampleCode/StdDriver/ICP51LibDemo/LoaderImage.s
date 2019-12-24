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
    EXPORT  SPROMImageBase
    EXPORT  SPROMImageLimit 

    ALIGN   4     
LDROMImageBase
    INCBIN .\LD_4KB.bin
LDROMImageLimit

    ALIGN   4
APROMImageBase
    INCBIN .\AP_GPIO.bin
APROMImageLimit

    ALIGN   4
SPROMImageBase
    INCBIN .\SP_128B.bin
SPROMImageLimit

    END