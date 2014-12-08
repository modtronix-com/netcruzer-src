/**
 * Application Configuration functions
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2011-08-16, David Hosken (DH):
 *  - Initial version, modified version of Microchip's spiFlash.h file from their TCP/IP stack
 *********************************************************************/
#ifndef __SPIFLASH_H
#define __SPIFLASH_H

#define SPI_FLASH_SECTOR_SIZE		(4096ul)
//MODTRONIX commented line below
//#define SPI_FLASH_PAGE_SIZE		(0ul)		// SST has no page boundary requirements
//MODTRONIX added line below
#define SPI_FLASH_PAGE_SIZE		(256ul)		// Winbond FLASH has 256 byte pages

#define SPI_FLASH_SECTOR_MASK		(SPI_FLASH_SECTOR_SIZE - 1)
//MODTRONIX added line below
#define SPI_FLASH_PAGE_MASK		(SPI_FLASH_PAGE_SIZE - 1)


#if defined(SPIFLASH_CS_TRIS)
	void spiFlashInit(void);		
	void spiFlashReadArray(DWORD dwAddress, BYTE *vData, WORD wLen);
	void spiFlashBeginWrite(DWORD dwAddr);
	void spiFlashWrite(BYTE vData);
	void spiFlashWriteArray(BYTE *vData, WORD wLen);
        void spiFlashEraseSector(DWORD dwAddr);
#else
	// If you get any of these linker errors, it means that you either have an 
	// error in your HardwareProfile.h or TCPIPConfig.h definitions.  The code 
	// is attempting to call a function that can't possibly work because you 
	// have not specified what pins and SPI module the physical SPI Flash chip 
	// is connected to.  Alternatively, if you don't have an SPI Flash chip, it 
	// means you have enabled a stack feature that requires SPI Flash hardware.
	// In this case, you need to edit TCPIPConfig.h and disable this stack 
	// feature.  The linker error tells you which object file this error was 
	// generated from.  It should be a clue as to what feature you need to 
	// disable.
	void You_cannot_call_the_spiFlashInit_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_spiFlashReadArray_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_spiFlashBeginWrite_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_spiFlashWrite_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_spiFlashWriteArray_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_spiFlashEraseSector_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	#define spiFlashInit()				You_cannot_call_the_spiFlashInit_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define spiFlashReadArray(a,b,c)	You_cannot_call_the_spiFlashReadArray_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define spiFlashBeginWrite(a)		You_cannot_call_the_spiFlashBeginWrite_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define spiFlashWrite(a)			You_cannot_call_the_spiFlashWrite_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define spiFlashWriteArray(a,b)		You_cannot_call_the_spiFlashWriteArray_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define spiFlashEraseSector(a)		You_cannot_call_the_spiFlashEraseSector_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
#endif

#endif
