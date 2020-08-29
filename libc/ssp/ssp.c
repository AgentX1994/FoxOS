#include <stdint.h>
#include <stdlib.h>

#if __is_libk
#include <kernel/panic.h>
#endif

// TODO make this randomized somehow
#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xf32a9883
#else
#define STACK_CHK_GUARD 0x9300acddba0911bc
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn)) void __stack_chk_fail(void)
{
#if __is_libc
    abort();
#elif __is_libk
    panic("Stack smashing detected");
#endif
}
