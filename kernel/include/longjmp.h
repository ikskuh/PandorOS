#pragma once

#include "config.h"

typedef __jmp_buf jmp_buf;

/**
 * @return The value passed to longjmp or 0 if direct invocation
 */
extern int setjmp(jmp_buf env);

extern void longjmp(jmp_buf env, int value) __attribute__ ((noreturn));
