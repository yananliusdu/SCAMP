/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2020
 * Generated linker script file for LPC4357-M0
 * Created from linkscript.ldt by FMCreateLinkLibraries
 * Using Freemarker v2.3.23
 * LPCXpresso v8.2.2 [Build 650] [2016-09-09]  on 2020-10-16 10:26:53
 */

INCLUDE "example_5_motion_detection_Release_library.ld"
INCLUDE "example_5_motion_detection_Release_memory.ld"

ENTRY(ResetISR)

SECTIONS
{
    /* MAIN TEXT SECTION */
    .text : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ; 
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        __bss_section_table_end = .;
        __section_table_end = . ;
	    /* End of Global Section Table */

        *(.after_vectors*)

    } >MFlashB512

    .text : ALIGN(4)    
    {
        *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);
			/* C++ constructors etc */
			. = ALIGN(4);
			KEEP(*(.init))
			
			. = ALIGN(4);
			__preinit_array_start = .;
			KEEP (*(.preinit_array))
			__preinit_array_end = .;
			
			. = ALIGN(4);
			__init_array_start = .;
			KEEP (*(SORT(.init_array.*)))
			KEEP (*(.init_array))
			__init_array_end = .;
			
			KEEP(*(.fini));
			
			. = ALIGN(4);
			KEEP (*crtbegin.o(.ctors))
			KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
			KEEP (*(SORT(.ctors.*)))
			KEEP (*crtend.o(.ctors))
			
			. = ALIGN(4);
			KEEP (*crtbegin.o(.dtors))
			KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
			KEEP (*(SORT(.dtors.*)))
			KEEP (*crtend.o(.dtors))
			. = ALIGN(4);
			/* End C++ */
    } > MFlashB512
    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this. 
     */
    .ARM.extab : ALIGN(4) 
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > MFlashB512
    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > MFlashB512
    __exidx_end = .;

    _etext = .;
        
    /* MAIN DATA SECTION */
    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > RamLoc40
    /* Main DATA section (RamLoc40) */
    .data : ALIGN(4)
    {
       FILL(0xff)
       _data = . ;
       *(vtable)
       *(.ramfunc*)
       *(.data*)
       . = ALIGN(4) ;
       _edata = . ;
    } > RamLoc40 AT>MFlashB512
    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > RamLoc40
    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*) 
         . = ALIGN(4) ;
        _end_noinit = .;
    } > RamLoc40

    PROVIDE(_pvHeapStart = DEFINED(__user_heap_base) ? __user_heap_base : .);
    PROVIDE(_vStackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_RamLoc40 - 0);
}