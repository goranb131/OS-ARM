SECTIONS {
  . = 0x80000000;
  .text : {
    *(.text.start)  
    *(.text*)
  }
  .rodata : { *(.rodata*) }
  .data : { *(.data*) }
  .bss : { *(.bss*) }

  . = ALIGN(16);
  .user_stack : {
    . = . + 0x10000;   /* 64 KB user stack (top at 0x80010000) */
  }
  user_stack_top = .;
}