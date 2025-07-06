/* user_shell.c  */

static inline void sys_putc(char c) {
    register long x0 asm("x0") = c;
    register long x8 asm("x8") = 1;
    asm volatile ("svc #0" : "+r"(x0) : "r"(x8) : "memory");
}
static inline char sys_getc(void) {
    register long x8 asm("x8") = 2;
    register long x0 asm("x0");
    asm volatile ("svc #0" : "=r"(x0) : "r"(x8) : "memory");
    return (char)x0;
}
void _user_start(void)
{
    sys_putc('$');
     //sys_putc('#');
    //sys_putc(' ');
    while (1) {
        char c = sys_getc();
        sys_putc(c);
    }
}