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

#include "duet_cm4.h"

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x20FFF000;   /* end of RAM */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size  = 0x10000;  /* required amount of heap  */
_Min_Stack_Size = 0x1000;   /* required amount of stack */

_vtable_size = (48*4);

/* Specify the memory areas */
MEMORY
{
FLASH (xr)          :     ORIGIN = APP_FLASH_START_ADDR, LENGTH = APP_MAX_SIZE
ITCM (xrw)          :     ORIGIN = 0x00080000, LENGTH = 32K
RAM (xrw)           :     ORIGIN = 0x20FD0000, LENGTH = 188K
#if defined(CFG_DUET_5822S)
RAM2 (xrw)          :     ORIGIN = 0x21000000, LENGTH = 96K
#else
RAM2 (xrw)          :     ORIGIN = 0x21000000, LENGTH = 0K
#endif
SHARED_MEMORY(rw)   :     ORIGIN = 0x60000000, LENGTH = 32K
}

/* Define output sections */
SECTIONS
{
  /* for secureboot image cert */
  .cert :
  {
    /* cert arera at flash */
    KEEP(*(app_cert_sec))
    . = ALIGN(0x1000);
  } >FLASH AT>FLASH

  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = 0x100;
  } >FLASH

  .app_version_sec :
  {
    KEEP(*(app_version_sec))
    . = ALIGN(0x10);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    PROVIDE(_flash_text_start = .);
    *(.text)           /* .text sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)
    *(.text*)          /* .text* sections (code) */
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
    PROVIDE(_flash_text_end = .);
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

  /* shared RAM */
  sharemem(NOLOAD):
  {
    . = ALIGN(4);
    _shmem_s = .;
    *(SHAREDRAM)
    . = ALIGN(4);
    _shmem_e = .;
  } >SHARED_MEMORY

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(CODE_DTCM)
    . = ALIGN(4);
    _edata = .;
  } >RAM  AT>FLASH

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
    . = ALIGN(8);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM


  /* vtable_section */
  .vtable_section	:
  {
    . = ALIGN(4);
    PROVIDE(_vtable_start = .);
    . = . + _vtable_size;
    PROVIDE(_vtable_end = .);
  } >ITCM AT>ITCM

  /* used by the startup to initialize data */
  _sifunc = LOADADDR(.func);

  /* Initialized func sections goes into ROM, load LMA copy after code */
  .func :
  {
    . = ALIGN(4);
    PROVIDE(_itcm_start = .);
    _sfunc = .;        /* create a global symbol at func start */
    *(seg_flash_driver)
    *(seg_wf_flash_driver)
    . = ALIGN(8);
    PROVIDE(_itcm_end = .);
    _efunc = .;        /* define a global symbol at func end */
  } >ITCM  AT>FLASH

  /* Must the last section in FLASH */
  .partition :
  {
    . = ALIGN(4);
    KEEP(*(ota_max_size_sec))
  } >FLASH

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  /* User_heap_stack section, used to check that there is enough RAM left */
  PROVIDE(end  = _ebss);
  PROVIDE(_end = _ebss);

  /* system stack */
  PROVIDE(_stack_base  = _estack - _Min_Stack_Size); /* _estack is top of stack*/
  ASSERT ((_stack_base > end), "Error: No room left for the stack")
  /* _estack is top of stack*/

  PROVIDE(itcm_heap_start = _itcm_end);
  PROVIDE(itcm_heap_len   = ORIGIN(ITCM) + LENGTH(ITCM) - itcm_heap_start);

  PROVIDE(dtcm_heap_start = _ebss);
  PROVIDE(dtcm_heap_len   = _stack_base - dtcm_heap_start);

  PROVIDE(sram_heap_start = ORIGIN(RAM2));
  PROVIDE(sram_heap_len   = LENGTH(RAM2));

  PROVIDE(heap_len   = (itcm_heap_len + dtcm_heap_len + sram_heap_len));
  ASSERT ((heap_len > _Min_Heap_Size), "Error: No room left for the heap")

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
