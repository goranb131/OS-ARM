OUTPUT_ARCH(aarch64)

#ifdef RPI4_BUILD
  . = 0x80000;
#else
  . = 0x40000000;
#endif

ENTRY(_start)

SECTIONS
{
  .text : { *(.text*) }
  .rodata : { *(.rodata*) }
  .data : { *(.data*) }
  .bss : {
    _bss_start = .;
    *(.bss*)
    *(COMMON)
    _bss_end = .;
  }

  . = ALIGN(16);

  /* KERNEL STACK way above */
  . = . + 0x100000;     /* skip 1MB after BSS (kernel stack at 0x40000000+kernel_size+1MB) */
  .stack : {
    . = . + 0x10000;    /* 64KB kernel stack (was 16K) */
  }
  stack_top = .;

  /* reserve 256MB before user code for safety */
  . = 0x80000000;
}