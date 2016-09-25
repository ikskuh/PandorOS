#pragma once
#define HAL x86
#include "../hal/x86/config.h"
#if !defined(PMM_MEMSIZE)
#error "PMM_MEMSIZE must be defined!"
#endif
#if !defined(PMM_PAGESIZE)
#error "PMM_PAGESIZE must be defined!"
#endif
