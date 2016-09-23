#pragma once
#include <stdarg.h>

/**
 * Generic printf.
 */
int gprintf(void (*putc)(char c), char const *fmt, va_list list);