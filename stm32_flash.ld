ENTRY(reset_handler)

_estack = 0x20010000;    /* end of 64K RAM */

_Min_Stack_Size = 0x200; /* required amount of stack */

MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 64K
  MEMORY_B1 (rx)  : ORIGIN = 0x60000000, LENGTH = 0K
}

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)  
    *(.text*)         
    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)        
    *(.rodata*)       
    . = ALIGN(4);
  } >FLASH

  _sidata = .;

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        
    *(.data)  
    *(.data*)        
    . = ALIGN(4);
    _edata = .;      
  } >RAM AT> FLASH

  . = ALIGN(4);
  .bss :
  {
    _sbss = .;         
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    . = ALIGN(4);
    _ebss = .;         
    __bss_end__ = _ebss;
  } >RAM

  PROVIDE ( end = _ebss );
  PROVIDE ( _end = _ebss );

  __mm_pool_start = .;
  __mm_pool_end = _estack - _Min_Stack_Size;
}
