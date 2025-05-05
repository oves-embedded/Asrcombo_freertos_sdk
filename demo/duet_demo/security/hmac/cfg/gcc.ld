/*
 * Copyright (c) 2022 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x21018000;    /* end of RAM */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x3000;      /* required amount of heap  */
_Min_Stack_Size = 0x0400; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      :  ORIGIN = 0x10012000, LENGTH = 256k
    RAM1 (xrw)      :  ORIGIN = 0x21000000, LENGTH = 8k
    RAM (xrw)       :  ORIGIN = 0x21002000, LENGTH = 88k
}

/* Define output sections */
SECTIONS
{

/* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
  } >FLASH
  
  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM  AT>FLASH

  /* used by the startup to initialize data */
  _sifunc = LOADADDR(.func);

  /* Initialized func sections goes into ROM, load LMA copy after code */
  .func :
  {
    . = ALIGN(4);
    _sfunc = .;        /* create a global symbol at func start */
    *(seg_flash_driver);
    . = ALIGN(4);
    *(seg_critical_function)          /* .func sections */
    . = ALIGN(4);
    _efunc = .;        /* define a global symbol at func end */
  } >RAM1  AT>FLASH

  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  /* User_heap_stack section, used to check that there is enough RAM left */
  . = ALIGN(8);
  PROVIDE ( end  = . );
  PROVIDE ( _end = . );

/* system stack */
  PROVIDE (_stack_base  = _estack - _Min_Stack_Size); /* _estack is top of stack*/
  ASSERT  ((_stack_base > end), "Error: No room left for the stack")
  /* _estack is top of stack*/

  /* left ram for heap */
  PROVIDE (heap_start = _end);
  PROVIDE (heap_end   = _stack_base);
  PROVIDE (heap_len   = heap_end - heap_start);
  ASSERT  ((heap_len > _Min_Heap_Size), "Error: No room left for the heap")

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
