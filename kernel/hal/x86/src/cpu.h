#pragma once

#include <stdint.h>

struct cpu
{
#define REGISTER(reg) uint32_t reg;
#include "registers.h"
#undef REGISTER
} __attribute__((packed));
