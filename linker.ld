OUTPUT_ARCH(aarch64)

#ifdef RPI4_BUILD
  /* build for RPI at 0x80000 */
  . = 0x80000;
#else
  /* Qemu at 0x40000000 */
  . = 0x40000000;
#endif

ENTRY(_start)

SECTIONS
{
  /* Kernel code and read‐only data */
  .text : {
    *(.text*)
  }

  .rodata : {
    *(.rodata*)
  }

  /* Initialized and uninitialized data */
  .data : {
    *(.data*)
  }

  .bss : {
    _bss_start = .;
    *(.bss*)
    *(COMMON)
    _bss_end = .;
  }

  . = ALIGN(16);


  /*  user_stack_bottom = low address
      user_stack_top    = high address */
  user_stack_bottom = .;
  . += 0x4000;           
  user_stack_top    = .;

  . = ALIGN(16);

  /* 
       stack_top = high address 
  */
  stack_top = . + 0x4000;
}