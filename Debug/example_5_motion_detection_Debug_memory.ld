/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2020
 * Generated linker script file for LPC4357-M0
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * LPCXpresso v8.2.2 [Build 650] [2016-09-09]  on 2020-10-8 23:12:55
 */

MEMORY
{
  /* Define each memory region */
  MFlashB512 (rx) : ORIGIN = 0x1b000000, LENGTH = 0x80000 /* 512K bytes (alias Flash) */  
  RamLoc40 (rwx) : ORIGIN = 0x10080000, LENGTH = 0xa000 /* 40K bytes (alias RAM) */  
}

  /* Define a symbol for the top of each memory region */
  __base_MFlashB512 = 0x1b000000  ; /* MFlashB512 */  
  __base_Flash = 0x1b000000 ; /* Flash */  
  __top_MFlashB512 = 0x1b000000 + 0x80000 ; /* 512K bytes */  
  __top_Flash = 0x1b000000 + 0x80000 ; /* 512K bytes */  
  __base_RamLoc40 = 0x10080000  ; /* RamLoc40 */  
  __base_RAM = 0x10080000 ; /* RAM */  
  __top_RamLoc40 = 0x10080000 + 0xa000 ; /* 40K bytes */  
  __top_RAM = 0x10080000 + 0xa000 ; /* 40K bytes */  
